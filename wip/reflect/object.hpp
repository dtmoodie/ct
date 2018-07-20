#pragma once

struct Foo
{
    float x, y, z;
};

class Bar
{
    float data;
public:
    const float& get() const{return data;}
    void set(const float val){data = val;}

    float half() const{return data / 2.0f;}
};

struct NonSerializable
{
    float a,b,c;
};

struct TestNonSerizableData
{
    NonSerializable a;
    float b;
};
