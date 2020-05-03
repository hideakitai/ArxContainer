#include <ArxContainer.h>

ArxRingBuffer<uint8_t, 4> buffer;

void setup()
{
    Serial.begin(115200);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);

    for(size_t i = 0; i < buffer.size(); ++i)
        Serial.println(buffer[i]);

    Serial.println();
    buffer.pop();

    for(auto& b : buffer)
        Serial.println(b);

    Serial.println();
    buffer.pop();

    for(auto& b : buffer)
        Serial.println(b);
}

void loop()
{
}
