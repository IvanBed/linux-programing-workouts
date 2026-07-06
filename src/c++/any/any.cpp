struct ICloneable
{
    virtual ICloneable* clone() const = 0;
    virtual ~ICloneable() { }
};

template <typename T>
struct ValueHolder: public ICloneable
{
    T _data;
       
    ValueHolder(const T& data) : _data(data) {}
    
    ValueHolder * clone() const
    {
        return new ValueHolder(_data);
    }    
};

class Any
{
private:
    ICloneable *_value;
public:
        
    Any()
    {
        _value = nullptr;
    }
    
    ICloneable const *get_value() const 
    {
        return this->_value;
    }

    template <typename T>
    Any(const T& value) : _value(new ValueHolder<T>(value)) {}
    
    ~Any()
    {
        delete _value;
    }

    Any(const Any &any)
    {
        ICloneable const *val = any.get_value();
        if (val)
            _value = val->clone();
        else 
            _value = nullptr;
    }

    Any & operator = (const Any & other)
    {
        ICloneable const *val;
        if (this != &other)
        {
            Any(other).swap(*this);
        }
        return *this;
    }
 
    void swap(Any &inst) noexcept
    {
        std::swap(_value, inst._value);
    }    
 
    template <typename T>
    T *cast()
    {
        ValueHolder<T> *vh_ptr = dynamic_cast<ValueHolder<T>*>(_value);
        if (!vh_ptr)
            return nullptr;
        else 
            return &vh_ptr->_data;
    }   
};
