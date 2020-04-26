# ArxRingBuffer
ArxRingBuffer for Arduino (replacement of `std::queue`, `std::deque` or `std::vector`).
AVR boards (like Uno and Mega) can't use `std::queue`, `std::deque` or `std::vector`.
This library is simple (and limited) replacement of such STLs.

## Usage

### Simple

``` C++
#include <ArxRingBuffer.h>
ArxRingBuffer<uint8_t, 4> buffer;

void setup()
{
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);

    for(size_t i = 0; i < buffer.size(); ++i)
        Serial.println(buffer[i]);

    buffer.pop();

    for(auto& b : buffer)
        Serial.println(b);

    buffer.clear();
}
```

### Original Data Class

``` C++
#include <ArxRingBuffer.h>
struct Data { int number; String str; };
ArxRingBuffer<Data, 3> buffer;

void setup()
{
    buffer.push(Data({1, "one"}));
    buffer.push(Data({2, "two"}));
    Data data = {3, "three"};
    buffer.push(data);

    for(size_t i = 0; i < buffer.size(); ++i)
    {
        Serial.print(buffer[i].number);
        Serial.print(", ");
        Serial.println(buffer[i].str);
    }

    buffer.pop();

    for(auto& b : buffer)
    {
        Serial.print(b.number);
        Serial.print(", ");
        Serial.println(b.str);
    }
}
```

## APIs

``` C++
size_t capacity() const
size_t size() const
bool empty() const

const T* data() const
T* data()

void push(const T& data)
void push(T&& data)
void push_back(const T& data)
void push_back(T&& data)

void pop()
void pop_front()
void pop_back()

const T& front() const
T& front()
const T& back() const
T& back()
const T& operator[] (uint8_t index) const
T& operator[] (uint8_t index)

const T* begin() const
T* begin()
const T* end() const
T* end()

void clear()
T* erase(T* p)
void resize(size_t sz)
void assign(const T* const first, const T* const end)
```

## License

MIT
