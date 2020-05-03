#include <ArxContainer.h>

arx::map<String, int> mp;

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // add contents
    mp.insert("one", 1);
    mp.insert("two", 2);
    mp.insert("three", 3);
    mp["four"] = 4;
    mp["five"] = 5;

    // range based access
    Serial.print({"{ "});
    for (const auto& m : mp)
    {
        Serial.print("{");
        Serial.print(m.first);
        Serial.print(",");
        Serial.print(m.second);
        Serial.print("},");
    }
    Serial.println({" }"});

    // key access
    Serial.print("one   = "); Serial.println(mp["one"]);
    Serial.print("two   = "); Serial.println(mp["two"]);
    Serial.print("three = "); Serial.println(mp["three"]);
    Serial.print("four  = "); Serial.println(mp["four"]);
    Serial.print("five  = "); Serial.println(mp["five"]);
}

void loop()
{
}
