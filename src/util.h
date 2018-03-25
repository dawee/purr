#ifndef PURR_UTIL_H
#define PURR_UTIL_H

static std::string ValueToSTDString(v8::Local<v8::Value> value) {
  v8::Local<v8::String> castedValue = value->ToString();
  v8::String::Utf8Value utf8Value(castedValue);
  std::string strValue(*utf8Value);

  return strValue;
}

#endif
