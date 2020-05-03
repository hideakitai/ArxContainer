#include <ArxContainer.h>

arx::deque<int> dq;

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // add contents
    for (int i = 0; i < 5; ++i)
        dq.push_back(i);

    // index access
    for (int i = 0; i < dq.size(); ++i)
    {
        Serial.print(dq[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void loop()
{
}
