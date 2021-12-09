#include <ArxContainer.h>

// initialize with initializer_list
arx::map<String, int> mp {{"one", 1}, {"two", 2}, {"four", 4}};

void setup() {
#ifdef NANO_OLD_BOOTLOADER // for rubbish nano clones with slow USB
    Serial.begin(38400);
#else
    Serial.begin(115200);
#endif
    delay(2000);

    // add contents
    mp.insert("three", 3);
    mp["five"] = 5;

    // range based access
    Serial.print({"{ "});
    for (const auto& m : mp) {
        Serial.print("{");
        Serial.print(m.first);
        Serial.print(",");
        Serial.print(m.second);
        Serial.print("},");
    }
    Serial.println({" }"});

    // key access
    Serial.print("one   = ");
    Serial.println(mp["one"]);
    Serial.print("two   = ");
    Serial.println(mp["two"]);
    Serial.print("three = ");
    Serial.println(mp["three"]);
    Serial.print("four  = ");
    Serial.println(mp["four"]);
    Serial.print("five  = ");
    Serial.println(mp["five"]);

    // const ref access
    /*
     * In file included from /home/xxxx/git/Arduino_ArxContainer/examples/map/map.ino:1:0:
/home/xxxx/git/yyyy/libraries/ArxContainer/ArxContainer.h: In instantiation of 'arx::map<Key, T, N>::const_iterator arx::map<Key, T, N>::find(const Key&) const [with Key = String; T = int; unsigned int N = 16; arx::map<Key, T, N>::const_iterator = const arx::RingBuffer<arx::pair<String, int>, 16>::Iterator]':
/home/xxxx/git/yyyy/libraries/ArxContainer/ArxContainer.h:709:20:   required from 'const T& arx::map<Key, T, N>::at(const Key&) const [with Key = String; T = int; unsigned int N = 16]'
/home/xxxx/git/Arduino_ArxContainer/examples/map/map.ino:50:34:   required from here
/home/xxxx/git/yyyy/libraries/ArxContainer/ArxContainer.h:659:47: warning: passing 'arx::RingBuffer<arx::pair<String, int>, 16>::const_iterator {aka const arx::RingBuffer<arx::pair<String, int>, 16>::Iterator}' as 'this' argument discards qualifiers [-fpermissive]
             const_iterator it = this->begin() + i;
/home/xxxx/git/yyyy/libraries/ArxContainer/ArxContainer.h:103:18: note:   in call to 'arx::RingBuffer<T, N>::Iterator arx::RingBuffer<T, N>::Iterator::operator+(int) [with T = arx::pair<String, int>; unsigned int N = 16]'
         Iterator operator+(const int n) {
                  ^~~~~~~~
     */
    const auto& mp_ro = mp;   
    Serial.print("const map one   = ");
    Serial.println(mp_ro.at("one"));
}

void loop() {
}
