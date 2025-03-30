#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <random>
#include <functional>
#include <vector>
#include <stack>
struct someObject
{
private:
    int field_1;
    char field_2;
    bool owned;
public:
    someObject(int f1, char f2) : field_1(f1), field_2(f2), owned(true) {}
    someObject() : field_1(10), field_2('z'), owned(true) {}

    int getField1() const
    {
        return field_1;
    }
    char getField2() const
    {
        return field_2;
    }
    void setField1(int field)
    {
        field_1 = field;
    }
    void setField2(char field)
    {
        field_2 = field;
    }
    ~someObject() {
        if (owned) {}
    }
    void setOwned(bool own) { owned = own; }
    bool getOwned()
    {
        return owned;
    }
    bool operator<(const someObject& temp) const {
        return field_1 < temp.field_1;
    }

    bool operator>(const someObject& temp) const {
        return field_1 > temp.field_1;
    }
};

template <typename T>
struct arr
{
    int currentSize;
    int maxSize;
    T* array;

    arr() : currentSize(0), maxSize(10), array(new T[10]) {}
    ~arr() {
        deleteAll();
        delete[] array;
    }

    //a
    void addTail(const T& temp)
    {
        if (currentSize >= maxSize)
        {
            resize();
        }
        array[currentSize++] = temp;
    }

    void resize()
    {
        maxSize *= 2;
        T* temp = new T[maxSize];
        for (int i = 0; i < currentSize; ++i)
        {
            temp[i] = array[i];
        }
        delete[] array;
        array = temp;
    }
    //b
    T findByIndex(int index)
    {
        if (index >= currentSize || index < 0)
        {
            throw std::out_of_range("Out of bounds");
        }
        return array[index];
    }
    //c
    void replaceByIndex(int index, const T& temp)
    {
        if (index >= currentSize || index < 0)
        {
            throw std::out_of_range("Out of bounds");
        }
        array[index] = temp;
    }
    //e
    void deleteAll()
    {
        if constexpr (std::is_pointer<T>::value) {

            for (int i = 0; i < currentSize; ++i) {
                if (array[i] != nullptr && array[i]->getOwned())
                    delete array[i];
            }
        }
        currentSize = 0;
    }

    void bubbleSort() {
        for (int i = 0; i < currentSize - 1; ++i) {
            for (int j = 0; j < currentSize - i - 1; ++j) {
                bool swap = false;
                if constexpr (std::is_pointer<T>::value)
                {
                    //dereferencja zeby nie porownywac adresow
                    swap = *array[j] > *array[j + 1];
                }
                else {
                    swap = array[j] > array[j + 1];
                }
                if (swap) {
                    std::swap(array[j], array[j + 1]);
                }
            }
        }
    }
    //e
    std::string toString()
    {
        int max = 5;
        std::ostringstream output;
        for (int i = 0; i < max && i < currentSize; ++i)
        {
            if constexpr (std::is_arithmetic<T>::value) {
                output << "(" << array[i] << ") ";
            }
            else if constexpr (std::is_pointer<T>::value) 
            {
                output << "(" << array[i]->getField1() << " , " << array[i]->getField2() << ") ";

            }
            else {
                output << "(" << array[i].getField1() << " , " << array[i].getField2() << ") ";
            }
        }
        if (currentSize > 10)
        {
            output << "... ";
        }
        if (currentSize >= 5)
        {
            for (int i = currentSize - max; i < currentSize; ++i)
            {
                if constexpr (std::is_arithmetic<T>::value) {
                    output << "(" << array[i] << ") ";
                }
                else if constexpr (std::is_pointer<T>::value)
                {
                    output << "(" << array[i]->getField1() << " , " << array[i]->getField2() << ") ";

                }
                else {
                    output << "(" << array[i].getField1() << " , " << array[i].getField2() << ") ";
                }
            }
        }
        return output.str();
    }
};

template <typename T>
struct heap {
    arr<T> data;

    void up(int index, std::function<bool(const T&, const T&)> comparator)
    {
        if (index == 0)
        {
            return;
        }

        int parent = (index - 1) / 2;
        if (comparator(data.array[index], data.array[parent]))
        {
            std::swap(data.array[index], data.array[parent]);
            up(parent, comparator);
        }
    }

    void down(int index, std::function<bool(const T&, const T&)> comparator)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int current = index;

        if (left < data.currentSize && comparator(data.array[left], data.array[current])) {
            current = left;
        }

        if (right < data.currentSize && comparator(data.array[right], data.array[current])) {
            current = right;
        }

        if (current != index) {
            std::swap(data.array[index], data.array[current]);
            down(current, comparator);
        }
    }

    void add(const T& element, std::function<bool(const T&, const T&)> comparator) {
        data.addTail(element);
        up(data.currentSize - 1, comparator);
    }

    void clear() {
        data.deleteAll();
    }

    std::string toString() {
        return data.toString();
    }

    T poll(std::function<bool(const T&, const T&)> comparator) {
        if (data.currentSize == 0) {
            throw std::out_of_range("Empty");
        }
        T max = data.array[0];
        data.array[0] = data.array[data.currentSize - 1];
        --data.currentSize;
        down(0, comparator);
        return max;
    }
};

template <typename T>
std::function<bool(const T&, const T&)> create() {
    if constexpr (std::is_arithmetic<T>::value)
    {
        return [](const T& a, const T& b)
            {
                return a > b;
            };
    }
    else if constexpr (std::is_pointer<T>::value)
    {
        return [](const T& a, const T& b)
            {
                return a->getField1() > b->getField1();
            };
    }
    else
    {
        return [](const T& a, const T& b)
            {
                return a.getField1() > b.getField1();
            };
    }
}

int main()
{
    auto ptrComp = create<someObject*>();
    srand(time(0));
    char letters[] = "abcdefghijklmnopqrstuvwxyz";
    const int MAX_ORDER = 1;
    heap<someObject*>* h = new heap<someObject*>();
    for (int o = 1; o <= MAX_ORDER; o++)
    {
        const int n = pow(10, o);
        clock_t t1 = clock();
        for (int i = 0; i < n; i++)
        {
            int x = ((rand() << 15) + rand()) % 5;
            char y = letters[rand() % 26];
            someObject* temp = new someObject(x, y);
            h->add(temp, ptrComp);
        }
        clock_t t2 = clock();
        double t3 = (t2 - t1) / (double)CLOCKS_PER_SEC;
        std::ostringstream out;
        std::cout << "/////////////////////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "Size: " << n << std::endl;
        std::cout << "After add: " << std::endl;
        std::cout << h->toString() << std::endl;
        std::cout << "Total add time: " << n << " elements: " << t3 << "s" << std::endl;
        std::cout << "Average add time: " << t3 / n * 1000000 << "us" << std::endl;

        t1 = clock();
        for (int i = 0; i < n; i++)
        {
            someObject* temp = h->poll(ptrComp);
            std::cout << temp->getField1() << temp->getField2() << std::endl;
            delete temp;
        }
        t2 = clock();
        t3 = (t2 - t1) / (double)CLOCKS_PER_SEC;
        std::cout << "After delete: " << std::endl;
        std::cout << h->toString() << std::endl;
        std::cout << "Total delete time: " << n << " elements: " << t3 << "s" << std::endl;
        std::cout << "Average delete time: " << t3 / n * 1000000 << "us" << std::endl;
        h->clear();
    }
    delete h;
    return 0;
}

