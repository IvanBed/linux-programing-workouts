#include <iostream>
#define cnt_increment(obj_counter_) *(obj_counter_) = *(obj_counter_) +  1
#define cnt_decrement(obj_counter_) *(obj_counter_) = *(obj_counter_) -  1
#define cnt_init(obj_counter_) *obj_counter_ = 1

using namespace std;

struct Expression
{
    virtual double evaluate() const = 0;
    virtual ~Expression() { }
};

struct Number : Expression
{
    Number(double value)
        : value(value)
    {}
    double evaluate () const 
    {
       
        return value;
    }
private:
    double value;
};

struct SharedPtr
{
private:
    Expression *ptr_;
    size_t *obj_counter_;
public:    
    explicit SharedPtr(Expression *ptr = 0) 
    {
        ptr_ = ptr;
        obj_counter_ = new size_t;
        cnt_init(obj_counter_);
    }
    
    SharedPtr(const SharedPtr& ptr)
    {
        ptr_ = ptr.get();
        obj_counter_ = ptr.get_current_obj_count();
        cnt_increment(obj_counter_);
    }
    
    ~SharedPtr()
    {
        if ((*obj_counter_) == 0) {
            delete ptr_;
            delete obj_counter_;  
        }
        else 
        {
            cnt_decrement(obj_counter_);
        }
    }
    
    Expression * get() const 
    {
        return ptr_;
    }
    
    size_t * get_current_obj_count() const
    {
        return obj_counter_;
    }
    
    void reset(Expression *ptr = 0) 
    {
        if ((*obj_counter_) == 0) {
            delete ptr_;
            delete obj_counter_;  
        }
        else 
        {
            cnt_decrement(obj_counter_);
        }
        ptr_ = ptr;
        obj_counter_ = new size_t;
        cnt_init(obj_counter_);
    }
    
    Expression & operator*()
    {
        return *ptr_;
    }
    
    Expression * operator->()
    {
        return ptr_;
    }
    
    SharedPtr & operator=(const SharedPtr& ptr) 
    {   
        if (this != &ptr)
        {
            if (*(obj_counter_) > 0)
                cnt_decrement(obj_counter_);
            else
            {
                delete ptr_;
            }    
            
            if (ptr.get() != nullptr)
            {
                ptr_ = ptr.get();
                obj_counter_ = ptr.get_current_obj_count();
                cnt_increment(obj_counter_);
            }
        }
        return *this;
    }
    
};

void test(SharedPtr *ptr)
{
    
    SharedPtr sp2(*ptr);
    SharedPtr sp3(*ptr);
    SharedPtr sp4(*ptr);
    sp4 = sp3;
    cout << *(ptr->get_current_obj_count()) << std::endl;
}

int main() {

    Number *num = new Number(2.0);
    Number *num2 = new Number(25.0);
    
    SharedPtr sp1(num);
    test(&sp1);
    
    cout << *(sp1.get_current_obj_count());
}
