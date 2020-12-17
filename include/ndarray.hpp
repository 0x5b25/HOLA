#pragma once


#include <vector>
#include <memory>
#include <cassert>

namespace HOLA
{
    class mapper;

    class _nditer
    {
        std::vector<int> _curr;
        std::vector<int> _tgt;

        bool _reached()
        {
            //bool reached = true;
            //for(int i = 0; i < _curr.size(); i++)
            //{
            //    if(_curr[i] != _tgt[i])
            //    {
            //        reached = false;
            //        break;
            //    }
            //}
            //
            //return reached;
            return _curr[0] >= _tgt[0];
        }

        void _inc()
        {
            _curr[_ndim() - 1] += 1;
            for(int i = _ndim() - 1; i > 0; i--)
            {
                if(_curr[i] >= _tgt[i])
                {
                    _curr[i] = 0;
                    _curr[i - 1] += 1;
                }
                else break;
            }
        }

        int _ndim() { return _tgt.size(); }

    public:
        _nditer() = default;

        _nditer(const std::vector<int>& dims)
        {
            _tgt = dims;
            _curr = std::vector<int>(_ndim(), 0);
        }

        const std::vector<int>& get() const { return _curr; }
        bool isLast() { return _reached(); }
        void next()
        {
            _inc();
        }

    };

    class mapping_info
    {
#define RANGE_CONV(x, len) ((x) = ( ((x) < 0) ? (x) + len:(x) ))

        friend class mapper;
    public:
        enum class Type
        {
            trivial, single, multiple, range, all
        };
    protected:
        union {
            int mapping;
            int* mappings;
            struct
            {
                int from, to;
            } range;
        } _data;

         Type _type;

        std::size_t _len;
    public:
        mapping_info(const mapping_info& other)
            :_type(other.type()),
            _len(other.size())
        {
            switch (other.type()) {
                //case Type::trivial: break;
                //case Type::single: break;
                case Type::multiple:
                    {
                        _data.mappings = new int[_len];
                        memcpy(_data.mappings, other._data.mappings, _len*sizeof(int));
                    }break;
                //case Type::range: break;
                //case Type::all: break;
                default: _data = other._data;
            }
        }

        mapping_info(mapping_info&& other)
            :_type(other.type()),
            _len(other.size())
        {
            switch (other.type()) {
                //case Type::trivial: break;
                //case Type::single: break;
            case Type::multiple:
            {
                _data.mappings = other._data.mappings;
                other._data.mappings = nullptr;
            }break;
            //case Type::range: break;
            //case Type::all: break;
            default: _data = other._data;
            }
        }

        mapping_info() { _data.mapping = 0; _type = Type::trivial; _len = 0; }

        mapping_info(int idx, bool is_trivial = false)
        {
            _data.mapping = idx;
            _type = is_trivial?Type::trivial:Type::single;
            _len = 1;
        }

        mapping_info(std::initializer_list<int> idx)
        {
            _len = idx.size();
            _data.mappings = new int[_len];
            std::copy(idx.begin(), idx.end(), _data.mappings);
            _type = Type::multiple;
        }

        mapping_info(const std::vector<int>& idx)
        {
            _len = idx.size();
            _data.mappings = new int[_len];
            std::copy(idx.begin(), idx.end(), _data.mappings);
            _type = Type::multiple;
        }

        static mapping_info all()
        {
            mapping_info m;
            m._type = Type::all;
            return m;
        }

        /*range mapping, [from, to) */
        static mapping_info range(int from, int to)
        {
            mapping_info m;
            m._type = Type::range;
            m._data.range = {from, to};
            return m;
        }

        ~mapping_info()
        {
            switch (_type)
            {
            case Type::trivial: break;
            case Type::single: break;
            case Type::multiple:
                {
                delete[] _data.mappings;
                } break;
            case Type::range: break;
            case Type::all: break;
            }
        }

        std::size_t size() const { return _len; }
        Type type() const { return _type; }
        std::size_t get(int idx) const
        {
            RANGE_CONV(idx, _len);
            switch (_type) {
            case Type::trivial:
            case Type::single: return _data.mapping;

            case Type::multiple:
            {
                assert(idx < _len);
                return _data.mappings[idx];
            }

            case Type::range: {
                assert(idx < _len);
                return _data.range.from + idx;
            }
            case Type::all: return idx;
            }
        }

        void _Install(int axis_len)
        {

            switch (_type) {
                case Type::trivial:
                case Type::single: RANGE_CONV(_data.mapping, axis_len); break;

                case Type::multiple:
                    {
                        for(int i = 0; i < _len; i++)
                        {
                            RANGE_CONV(_data.mappings[i], axis_len);
                        }
                    }break;

                case Type::range:
                    {
                        RANGE_CONV(_data.range.from, axis_len);
                        RANGE_CONV(_data.range.to, axis_len);
                        _len = _data.range.to - _data.range.from;
                        assert(_len >= 0);
                    }break;

                case Type::all: _len = axis_len; break;

            }

        }

        mapping_info& operator=(const mapping_info& other)
        {
            _type = other._type;
            _len = other._len;
            {
                switch (other.type()) {
                    //case Type::trivial: break;
                    //case Type::single: break;
                case Type::multiple:
                {
                    _data.mappings = new int[_len];
                    memcpy(_data.mappings, other._data.mappings, _len * sizeof(int));
                }break;
                //case Type::range: break;
                //case Type::all: break;
                default: _data = other._data;
                }
            }

            return *this;
        }

        mapping_info& operator=( mapping_info&& other)
        {
            _type = other._type;
            _len = other._len;
            {
                switch (other.type()) {
                    //case Type::trivial: break;
                    //case Type::single: break;
                case Type::multiple:
                {
                    _data.mappings = other._data.mappings;
                    other._data.mappings = nullptr;
                }break;
                //case Type::range: break;
                //case Type::all: break;
                default: _data = other._data;
                }
            }

            return *this;
        }
#undef  RANGE_CONV

    };


    class mapper
    {
        std::vector<mapping_info> _axis_mappings;

    public:
        mapper() = default;

        mapper(std::initializer_list<mapping_info> axes)
            :_axis_mappings(axes){}

        mapper(const std::vector<mapping_info>& axes)
            :_axis_mappings(axes){}

        mapper(std::vector<mapping_info>&& axes)
            :_axis_mappings(axes){}

        mapper(const std::vector<int>& dims)
        {
            _axis_mappings.reserve(dims.size());
            for(int i:dims)
            {
                _axis_mappings.push_back(mapping_info::all());
            }

            _Install(dims);
        }

        mapper(const mapper& other)
        {
            _axis_mappings = other._axis_mappings;
        }
        mapper(mapper&& other)
            :_axis_mappings(std::move(other._axis_mappings))
        {
            
        }


        void _Install(const std::vector<int>& axes)
        {
            for(int i = 0; i < _ndim(); i++)
            {
                _axis_mappings[i]._Install(axes[i]);
            }
        }

        mapper Remap(const std::vector<mapping_info>& newmap) const
        {
            assert(ndim() == newmap.size());
            auto t = newmap.begin();
            std::vector<mapping_info> actual(_ndim());
            for(int i = 0; i< _ndim(); i++)
            {
                if(_axis_mappings[i]._type == mapping_info::Type::trivial)
                {
                    actual[i] = _axis_mappings[i];
                    continue;
                }
                mapping_info m = (*t);
                ++t;
                m._Install(_axis_mappings[i].size());

                switch (m._type) {                    
                case mapping_info::Type::single:
                    actual[i] = 
                        mapping_info(
                            _axis_mappings[i].get(
                                m.get(0)
                            )
                        );

                    break;

                case mapping_info::Type::multiple:
                case mapping_info::Type::range:
                    {
                        auto& newentry = m;
                        auto& thisentry = _axis_mappings[i];
                        std::vector<int> realindices(newentry.size());
                        for(int e = 0; e < newentry.size(); e++)
                        {
                            realindices[e] = thisentry.get(newentry.get(e));
                        }
                        actual[i] = mapping_info(realindices);
                    }break;

                case mapping_info::Type::all: actual[i] = _axis_mappings[i]; break;

                }
            }

            auto res = mapper(actual);
            //res._Install(_shape());
            return res;
        }

        mapper _Subarray(const std::vector<int>& dims) const
        {
            assert(_ndim() >= dims.size());
            std::vector<mapping_info> actual(_ndim());
            for (int i = 0; i < dims.size(); i++)
            {
                
                    actual[i] =
                        mapping_info(
                            _axis_mappings[i].get(
                                dims[i]
                            ),true
                        );
                                   
            }

            for (int i = dims.size(); i < _ndim(); i++)
            {
                actual[i] = _axis_mappings[i];
            }

            return mapper(actual);
        }


        mapper Subarray(const std::vector<int>& dims) const
        {
            assert(ndim() >= dims.size());
            std::vector<mapping_info> actual(_ndim());
            int i = 0;
            auto t = dims.begin();
            while(true)
            {
                if(t == dims.end()) break;

                if (_axis_mappings[i].type() == mapping_info::Type::trivial)
                {
                    actual[i] = _axis_mappings[i];
                }
                else {

                    actual[i] =
                        mapping_info(
                            _axis_mappings[i].get(
                                *t
                            ), true
                        );
                    t += 1;
                }
                i += 1;
                assert(i <= _ndim());
            }

            for (; i < _ndim(); i++)
            {
                actual[i] = _axis_mappings[i];
            }

            return mapper(actual);
        }

        /*actual dimension count, including trivial axes*/
        std::size_t _ndim() const
        {
            return _axis_mappings.size();
        }

        /*Visible dimension count*/
        std::size_t ndim() const
        {
            int d = 0;

            for (int i = 0; i < _ndim(); i++)
            {
                if (_axis_mappings[i].type() == mapping_info::Type::trivial) continue;
                d++;
            }

            return d;
        }

        const int _size(const int& dim) const
        {
            auto rdim = dim < 0 ? dim + _ndim() : dim;
            assert(rdim < _ndim());
            return _axis_mappings[rdim].size();
        }


        const int size(const int& dim) const
        {
            auto rdim = dim < 0 ? dim + _ndim() : dim;
            assert(rdim < _ndim());
            int p = -1;
            for (int i = 0; i < _ndim(); i++)
            {
                if (_axis_mappings[i].type() == mapping_info::Type::trivial) continue;

                p++;
                if(p == rdim)
                    return _axis_mappings[i].size();
            }

            return 0;
        }


        std::vector<int> _shape() const
        {
            std::vector<int> s;

            for (int i = 0; i < _ndim(); i++)
            {
                s.push_back(_axis_mappings[i].size());
            }

            return s;
        }

        std::vector<int> shape() const
        {
            std::vector<int> s;

            for(int i = 0; i < _ndim(); i++)
            {
                if(_axis_mappings[i].type() == mapping_info::Type::trivial) continue;
                s.push_back(_axis_mappings[i].size());
            }

            return s;
        }

        std::vector<int> _map(const std::vector<int>& indices) const
        {
            assert(indices.size() ==_ndim());
            std::vector<int> mapped(_ndim());

            for (int i = 0; i < _ndim(); i++)
            {
                mapped[i] = _axis_mappings[i].get(*(indices.begin() + i));
            }

            return mapped;

        }

        /*Map conserved axes to underlying buffer*/
        std::vector<int> map(const std::vector<int>& indices) const
        {
            assert(indices.size() == ndim());
            std::vector<int> mapped(_ndim());
            auto t = indices.begin();
            for (int i = 0; i < _ndim(); i++)
            {
                if (_axis_mappings[i].type() == mapping_info::Type::trivial)
                {
                    mapped[i] = _axis_mappings[i].get(0);
                }
                else {
                    mapped[i] = _axis_mappings[i].get(*t);
                    t += 1;
                }
            }

            return mapped;

        }

        const mapping_info& operator[](int idx)
        {
            idx = idx < 0 ? idx + ndim() : idx;

            assert(idx < ndim());

            int p = -1;
            for (int i = 0; i < _ndim(); i++)
            {
                if (_axis_mappings[i].type() == mapping_info::Type::trivial) continue;

                p++;
                if (p == idx)
                    return _axis_mappings[i];
            }

            
        }

        mapper& operator=(const mapper& other) = default;
        mapper& operator=(mapper&& other) = default;
    };
    

    template<class T>
    class _buffer
    {
        std::vector<T> _data;
        std::vector<int>_dims;

    public:
        _buffer() = default;

        template <class InputIterator>
        _buffer(InputIterator begin, InputIterator end) // Iterator-pair constructor
            :_dims(begin, end)
        {
            if (ndim() != 0)
            {

                std::size_t len = 1;
                for (auto i = 0; i < ndim(); i++)
                {
                    //len *= *(dims.begin() + i);
                    len *= _dims[i];
                    //_dims[i] = *(dims.begin() + i);
                }

                _data = std::vector<T>(len);
            }
        }

        _buffer(std::initializer_list<int> dims) :_buffer(dims.begin(), dims.end()){}
        _buffer(const std::vector<int>& dims)
            :_buffer(dims.begin(), dims.end())
        {
            
        }

        std::size_t ndim() const
        {
            return _dims.size();
        }

        const int size(const int& dim) const
        {
            assert(dim < ndim());
            return _dims[dim];
        }

        void view(const std::vector<int>& dims)
        {
            _dims = dims;
        }

        const std::vector<int>& shape() const { return _dims; }

        std::size_t _GetOffset(const std::vector<int>& dims) const
        {
            std::size_t offset = 0;

            for (int i = 0; i < dims.size(); i++)
            {
                assert(dims[i] < _dims[i]);
                offset = offset * _dims[i] + *(dims.begin() + i);
            }

            return offset;
        }

        T& at(const std::vector<int>& index)
        {
            return _data[_GetOffset(index)];
        }

        void CopyData(const std::vector<T>& from)
        {
            _data = from;
        }


    };

    template<class T>
    class ndarray
    {
    protected:
        std::shared_ptr< _buffer<T>> _data;

        mapper _dims;
    public:

        ndarray() = default;

        template <class InputIterator>
        ndarray(InputIterator begin, InputIterator end)
            :ndarray(std::vector<T>(begin, end))
        {
            

        }

        ndarray(const std::vector<T>& data)
        {
            _dims = std::move(mapper({ mapping_info::all() }));
            //_data = std::make_shared<_buffer<T>>({ data.size() });
            _data.reset(new _buffer<T>({ (int)data.size() }));
            _data->CopyData(data);

            _dims._Install({ (int)data.size() });

        }

        ndarray(std::initializer_list<ndarray<T>> subarrs)
            :ndarray(stack(subarrs))
        { }

        static ndarray<T> FromShape(const std::vector<int>& shape)
        {
            auto result = ndarray();
            result._data = std::make_shared<_buffer<T>>(shape);
            result._dims = mapper(shape);
            return result;
        }

        ndarray(const ndarray& other)
            :_data(other._data), _dims(other._dims){}

        ndarray(std::shared_ptr<_buffer<T>>& data, mapper& dims)
            :_data(data), _dims(dims) {}

        std::size_t ndim() const
        {
            return _dims.ndim();
        }


        const int size(const int& dim) const
        {
            assert(dim < ndim());
            return _dims.size(dim);
        }

        ndarray view()
        {
            throw "Not implemented.";
        }

        std::vector<int> shape() const { return _dims.shape(); }

        ~ndarray()
        {
        }

        ndarray<T> at(const std::vector<int> idx)
        {
            auto arr = ndarray();

            auto submapper = _dims.Subarray( idx );

            arr._data = _data;
            arr._dims = submapper;

            return arr;
        }

        ndarray<T> operator[](int idx)
        {
            auto arr = ndarray();

            auto submapper = _dims.Subarray({ idx });

            arr._data = _data;
            arr._dims = submapper;

            return arr;
        }

        T item() const
        {
            assert(shape().size() == 1 && shape()[0] == 1);
            auto idx = _dims.map({ 0 });
            return _data->at(idx);
        }

        ndarray<T> copy()
        {
            auto arr = ndarray(shape());
            arr = *this;
            return arr;
        }

        static ndarray<T> stack(const std::vector<ndarray<T>>& arrs)
        {
            auto thisdim = arrs.size();
            if (thisdim <= 0) return ndarray();
            auto arrshape = arrs[0].shape();
            std::vector<int> dims;
            dims.push_back(thisdim);
            dims.insert(dims.end(), arrshape.begin(), arrshape.end());

            ndarray<T> result = ndarray<T>::FromShape(dims);

            for(int i = 0; i < arrs.size(); i++)
            {
                assert(arrs[i].shape() == arrshape);
                result[i] = arrs[i];
            }

            return result;
        }

        ndarray<T> operator[](const std::vector<mapping_info>& slices)
        {
            auto actualmap = _dims.Remap(slices);
            

            ndarray<T> arr(_data,actualmap);
            //arr._data = _data;
            //arr._dims = actualmap;

            return arr;
        }

        //ndarray<T>& operator=(const T& val)
        //{
        //    auto it = _nditer(shape());
        //    while(true)
        //    {
        //        if (it.isLast()) break;
        //        auto indices = it.get();
        //        it.next();
        //
        //        auto indices_mapped = _dims.map(indices);
        //        _data->at(indices_mapped) = val;
        //
        //    }
        //
        //    return *this;
        //}
        //
        ndarray<T>& operator=(ndarray<T>&& arr)
        {
            assert(shape() == arr.shape());

            _data = std::move(arr._data);
            _dims = std::move(arr._dims);

            return *this;
        }

        

#define _ARR_IDX(a, indices) (a)._data->at((a)._dims.map(indices))
#define _SCL_IDX(a, indices) (a)


#define _NDARR_ITER(body, ...)                   \
    auto it = _nditer(shape());                  \
    while (true)                                 \
    {                                            \
        if (it.isLast()) break;                  \
        auto indices = it.get();                 \
        it.next();                               \
        body(indices, __VA_ARGS__)        \
    }

#define _NDARR_OP_PREP(rtype, opname, args, body) \
    rtype operator##opname(args){          \
        body                                     \
    }

#define _NDARR_OP_EXP_LOCAL(idx, op, IDX)         \
        _ARR_IDX(*this,idx) op IDX(val,idx);


#define _NDARR_OP_EXP_CPY(idx, op, IDX, res)\
        _ARR_IDX(res, idx) = _ARR_IDX(*this, idx) op IDX(val, idx);
    

#define _NDARR_OP_BODY_LOCAL(opname, args, IDX) \
    _NDARR_OP_PREP(ndarray<T>&, opname, args,          \
        _NDARR_ITER(_NDARR_OP_EXP_LOCAL, opname,IDX)                  \
        return *this;                     \
    )


#define _NDARR_OP_BODY_CPY(opname, args, IDX) \
    _NDARR_OP_PREP(ndarray<T>, opname, args,\
        ndarray<T> result = FromShape(shape());           \
        _NDARR_ITER(_NDARR_OP_EXP_CPY, opname,IDX, result)\
        return result;                          \
    )

#define _NDARR_OP_SCALAR(TYPE, op) TYPE(op,const T& val, _SCL_IDX)
#define _NDARR_OP_ARRAY(TYPE, op) TYPE(op,const ndarray<T>& val, _ARR_IDX)

#define _NDARR_OP_LOCAL(op) \
    _NDARR_OP_SCALAR(_NDARR_OP_BODY_LOCAL, op)\
    _NDARR_OP_ARRAY(_NDARR_OP_BODY_LOCAL, op)


#define _NDARR_OP_CPY(op) \
    _NDARR_OP_SCALAR(_NDARR_OP_BODY_CPY, op)\
    _NDARR_OP_ARRAY(_NDARR_OP_BODY_CPY, op)

        ndarray<T>& operator=(const T& val)
        {
            auto it = _nditer(shape());
            while (true)
            {
                if (it.isLast()) break;
                auto indices = it.get();
                it.next();
                (*this)._data->at((*this)._dims.map(indices)) = (val);
            }
            return *this;
        } _NDARR_OP_ARRAY(_NDARR_OP_BODY_LOCAL, =)
        _NDARR_OP_LOCAL(+=)
        _NDARR_OP_LOCAL(-=)
        _NDARR_OP_LOCAL(*= )
        _NDARR_OP_LOCAL(/=)
        
        _NDARR_OP_CPY(+)
        _NDARR_OP_CPY(-)
        _NDARR_OP_SCALAR(_NDARR_OP_BODY_CPY, *)
        ndarray<T> operator*(const ndarray<T>& val)
        {
            ndarray<T> result = FromShape(shape());
            auto it = _nditer(shape());
            while (true)
            {
                if (it.isLast()) break;
                auto indices = it.get();
                it.next();
                (result)._data->at((result)._dims.map(indices)) = 
                    (*this)._data->at((*this)._dims.map(indices)) * 
                    (val)._data->at((val)._dims.map(indices));
            } return result;
        }
        _NDARR_OP_CPY(/)

    protected:

        

    public:


        void Swap(const std::vector<int>& a, const std::vector<int>& b)
        {
            assert(a.size() == b.size() && a.size() <= ndim());
            auto arr_a = at(a);
            auto arr_b = at(b);

            auto temp = arr_a.copy();
            arr_a = arr_b;
            arr_b = temp;
        }

        
    };
}
