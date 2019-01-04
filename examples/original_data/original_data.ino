#include <RingBuffer.h>

struct Data { int number; String str; };
RingBuffer<Data, 3> buffer;

void setup()
{
    Serial.begin(115200);

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

    Serial.println();
    buffer.pop();

    for(auto& b : buffer)
    {
        Serial.print(b.number);
        Serial.print(", ");
        Serial.println(b.str);
    }

    Serial.println();
    buffer.pop();

    for(auto& b : buffer)
    {
        Serial.print(b.number);
        Serial.print(", ");
        Serial.println(b.str);
    }
}

void loop()
{
}
