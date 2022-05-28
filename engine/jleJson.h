#pragma once

// TODO: Stop using try-catch
#define JLE_FROM_JSON_DEFAULT(JSON, VAL, STR, DEF) \
try { VAL = JSON.at(STR); } catch (std::exception e) { VAL = DEF; }
// Example usage: JLE_FROM_JSON_DEFAULT(j_in, x, "x", 0.f);

template <class JSON>
class jleJsonInterface
{
public:
	virtual ~jleJsonInterface() {}
	virtual void ToJson(JSON& j_out) = 0;
	virtual void FromJson(const JSON& j_in) = 0;
};