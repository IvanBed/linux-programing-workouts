#include <cstddef>

template <typename T>
class Array
{
public:
    T * data;
    size_t size_;
 
    explicit Array(size_t size, const T& value = T()) 
    {
        size_ = size;
        
        data = (T*)new char[sizeof(T) * size];
        for (size_t i = 0; i < size; i++)
           new (data + i) T(value);     
    }

    Array()
    {
        data = nullptr;
        size_ = 0;
    }
           
    Array(const Array & arr)
    {
        T *new_data = (T*)new char[sizeof(T) * arr.size()];
        size_ = arr.size();
        
        for(size_t i = 0; i < size_; i++)
            new (new_data + i) T(arr[i]);  
        
        data = new_data;
    }
    
    ~Array()
    {
         for (size_t i = 0; i < size_; i++)
         {
              data[i].~T();
         }
         operator delete [] (data);
    }
    
    size_t size() const
    {
        return size_;
    }
    
    // Уточнить про перегрузку оператора сравнения
    Array & operator=(const Array & arr)
    {
        if(&arr != this)
        {
            T *new_data = (T*)new char[sizeof(T) * arr.size()];
            for(size_t i = 0; i < arr.size(); i++)
                new (new_data + i) T(arr[i]);
            
            for (size_t i = 0; i < size_; i++)
            {
                 data[i].~T();
            }
            operator delete [] (data); 
            data = new_data;
            size_ = arr.size();
        }
        return *this;
    }
 
    T & operator[](size_t i)
    {
        return data[i];
    }
    
    T operator[](size_t i) const
    {
        return data[i];
    }   
};
