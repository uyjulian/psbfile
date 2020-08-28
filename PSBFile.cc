/**
 * @file      PSBFile.cc
 * @author    Hikaru Terazono (3c1u) <3c1u@vulpesgames.tokyo>
 * @brief     The PSBFile.dll replacement for Kirikiri-Z.
 * @date      2020-08-13
 *
 * @copyright Copyright (c) 2020 Hikaru Terazono. All rights reserved.
 *
 */

#include "ncbind/ncbind.hpp"

#include "istream_compat.h"
#include "src/psb.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

// Use this for dump .PSB file
// #define DUMP_VALUE

extern bool TVPEncodeUTF8ToUTF16(tjs_string &output, const std::string &source);

// use for debugging
static void dump_psb_value(psb_t *psb, psb_value_t const *value, int indent = 0,
                           bool terminate_line = true);

static tTJSVariant convert_psb_value(psb_t *psb, psb_value_t const *value);

static void dump_psb_value(psb_t *psb, psb_value_t const *value, int indent,
                           bool terminate_line) {
  static std::vector<std::string> type_map{
      "psb_value_t",    "psb_null_t",    "psb_boolean_t",
      "psb_resource_t", "psb_number_t",  "psb_array_t",
      "psb_string_t",   "psb_objects_t", "psb_collection_t",
  };

  auto res =
      std::find(type_map.begin(), type_map.end(), value->get_type_string());

  switch (std::distance(type_map.begin(), res)) {
  case 1:
    std::cout << "<null>";
    break;
  case 2: {
    psb_boolean_t const *v = dynamic_cast<psb_boolean_t const *>(value);
    if (!v) {
      //
    }

    if (v->get_boolean())
      std::cout << "true";
    else
      std::cout << "false";
    break;
  }
  case 3:
    std::cout << "<resource>";
    break;
  case 4: {
    psb_number_t const *v = dynamic_cast<psb_number_t const *>(value);
    if (!v) {
      //
    }

    switch (v->get_number_type()) {
    case 0:
      std::cout << "int(" << v->get_integer() << ")";
      break;
    case 1:
      std::cout << "f32(" << v->get_float() << ")";
      break;
    case 2:
      std::cout << "f64(" << v->get_double() << ")";
      break;
    default:
      std::cout << "<invalid number>";
      break;
    }
    break;
  }
  case 5:
    std::cout << "[";
    {
      psb_array_t const *v = dynamic_cast<psb_array_t const *>(value);
      if (!v) {
        //
      }

      auto size = v->size();

      for (size_t i = 0; i < size; i++) {
        std::cout << v->get(i) << ", ";
      }
    }
    std::cout << "]";
    break;
  case 6: {
    psb_string_t const *v = dynamic_cast<psb_string_t const *>(value);
    if (!v) {
      //
    }
    std::cout << '"' << v->get_string() << '"';
    break;
  }
  case 7: {
    psb_objects_t const *v = dynamic_cast<psb_objects_t const *>(value);
    if (!v) {
      //
    }

    std::cout << "{" << std::endl;
    auto size = v->size();

    ++indent;

    for (size_t i = 0; i < size; i++) {
      for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
      }

      std::cout << v->get_name(i) << ": ";

      auto         ptr = v->get_data(i);
      psb_value_t *v2  = psb->unpack(ptr);

      dump_psb_value(psb, v2, indent, false);

      delete v2;
      std::cout << ", " << std::endl;
    }

    --indent;
    for (int i = 0; i < indent; ++i) {
      std::cout << "  ";
    }
    std::cout << "}";
    break;
  }
  case 8: {
    psb_collection_t const *v = dynamic_cast<psb_collection_t const *>(value);
    if (!v) {
      //
    }

    std::cout << "[" << std::endl;
    auto size = v->size();

    ++indent;

    for (size_t i = 0; i < size; i++) {
      for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
      }

      auto         ptr = v->get(i);
      psb_value_t *v2  = psb->unpack(ptr);

      dump_psb_value(psb, v2, indent, false);

      delete v2;

      std::cout << ", " << std::endl;
    }

    --indent;
    for (int i = 0; i < indent; ++i) {
      std::cout << "  ";
    }
    std::cout << "]";
    break;
  }
  case 0:
  default:
    std::cout << "<unknown value>";
    break;
  }

  if (terminate_line) {
    std::cout << std::endl;
  }
}

static tTJSVariant convert_psb_value(psb_t *psb, psb_value_t const *value) {
  static std::vector<std::string> type_map{
      "psb_value_t",    "psb_null_t",    "psb_boolean_t",
      "psb_resource_t", "psb_number_t",  "psb_array_t",
      "psb_string_t",   "psb_objects_t", "psb_collection_t",
  };

  auto res =
      std::find(type_map.begin(), type_map.end(), value->get_type_string());

  if (res == type_map.end()) {
    TVPThrowExceptionMessage(TJS_W("PSB invalid type: unrecognized type name"));
  }

  auto typenum = std::distance(type_map.begin(), res);

  switch (typenum) {
  case 1:
    return tTJSVariant();
    break;
  case 2: {
    psb_boolean_t const *v = dynamic_cast<psb_boolean_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected boolean"));
    }

    return tTJSVariant(v->get_boolean());
    break;
  }
  case 3: {
    // TODO: handle psb_resouce_t::index
    psb_resource_t const *v = dynamic_cast<psb_resource_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected resource"));
    }

    // the buffer is copied
    return tTJSVariant(v->get_buff(), v->get_length());

    break;
  }
  case 4: {
    psb_number_t const *v = dynamic_cast<psb_number_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected number"));
    }

    switch (v->get_number_type()) {
    case 0:
      return tTJSVariant(v->get_integer());
      break;
    case 1:
      return tTJSVariant(v->get_float());
      break;
    case 2:
      return tTJSVariant(v->get_double());
      break;
    default:
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: invalid number type"));
      break;
    }
    break;
  }
  case 5: {
    psb_array_t const *v = dynamic_cast<psb_array_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected array"));
    }

    auto arr  = TJSCreateArrayObject();
    auto size = v->size();

    for (size_t i = 0; i < size; i++) {
      tTJSVariant var(static_cast<tjs_int32>(v->get(i)));
      arr->PropSetByNum(TJS_MEMBERENSURE, i, &var, arr);
    }

    auto output = tTJSVariant(arr, arr);
    arr->Release();

    return output;

    break;
  }
  case 6: {
    psb_string_t const *v = dynamic_cast<psb_string_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected string"));
    }

    tjs_string str{};
    TVPEncodeUTF8ToUTF16(str, v->get_string());

    return ttstr(str);
  }
  case 7: {
    psb_objects_t const *v = dynamic_cast<psb_objects_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected objects"));
    }

    auto dict = TJSCreateDictionaryObject();
    auto size = v->size();

    for (size_t i = 0; i < size; i++) {
      tjs_string name{};
      TVPEncodeUTF8ToUTF16(name, v->get_name(i));

      auto         ptr   = v->get_data(i);
      psb_value_t *child = psb->unpack(ptr);

      tTJSVariant var = convert_psb_value(psb, child);

      dict->PropSet(TJS_MEMBERENSURE, name.c_str(), nullptr, &var, dict);

      delete child;
    }

    auto output = tTJSVariant(dict, dict);
    dict->Release();

    return output;

    break;
  }
  case 8: {
    psb_collection_t const *v = dynamic_cast<psb_collection_t const *>(value);
    if (!v) {
      TVPThrowExceptionMessage(
          TJS_W("PSB type conversion failed: expected collection"));
    }

    auto arr  = TJSCreateArrayObject();
    auto size = v->size();

    for (size_t i = 0; i < size; i++) {
      auto         ptr   = v->get(i);
      psb_value_t *child = psb->unpack(ptr);

      tTJSVariant var = convert_psb_value(psb, child);

      arr->PropSetByNum(TJS_MEMBERENSURE, i, &var, arr);

      delete child;
    }

    auto output = tTJSVariant(arr, arr);
    arr->Release();

    return output;

    break;
  }
  case 0:
  default:
    TVPThrowExceptionMessage(
        TJS_W("PSB invalid type: not supported typenum: %1"), tjs_int(typenum));
    break;
  }

  return tTJSVariant();
}

class PSBFile {
public:
  PSBFile(const tTJSVariant &filename)
      : m_psb(nullptr), m_buf(nullptr), m_root() {
    // load the file and load into a buffer
    open(filename);

    // load into psb_t
    m_psb = new psb_t(m_buf);

#ifdef DUMP_VALUE
    dump_psb_value(m_psb, m_psb->get_objects());
#endif
    m_root = convert_psb_value(m_psb, m_psb->get_objects());
  }

  ~PSBFile() {
    if (m_psb) {
      delete m_psb;
    }

    if (m_buf) {
      delete[] m_buf;
    }
  }

  tTJSVariant getRoot() { return m_root; }

private:
  psb_t *     m_psb;
  uint8_t *   m_buf;
  tTJSVariant m_root;

  void open(const tTJSVariant &filename) {

    auto in = TVPCreateIStream(filename, TJS_BS_READ);

    TVPAddLog(
        (ttstr(TJS_W("loading file: ")) + filename.AsStringNoAddRef()).c_str());

    if (!in) {
      TVPThrowExceptionMessage(
          (ttstr(TJS_W("cannot open : ")) + filename.AsStringNoAddRef())
              .c_str());
    }

    ULARGE_INTEGER filesize;
    if (in->Seek({0}, STREAM_SEEK_END, &filesize) != S_OK) {
      TVPThrowExceptionMessage(
          (ttstr(TJS_W("cannot seek : ")) + filename.AsStringNoAddRef())
              .c_str());
    }

    if (in->Seek({0}, STREAM_SEEK_SET, nullptr) != S_OK) {
      TVPThrowExceptionMessage(
          (ttstr(TJS_W("cannot seek : ")) + filename.AsStringNoAddRef())
              .c_str());
    }

    auto total_size = filesize.QuadPart;
    auto buf        = new uint8_t[total_size];

    if (in->Read(reinterpret_cast<void *>(buf), total_size, &total_size) !=
            S_OK ||
        total_size != filesize.QuadPart) {
      delete[] buf;

      TVPThrowExceptionMessage(
          (ttstr(TJS_W("failed to read: ")) + filename.AsStringNoAddRef())
              .c_str());
    }

    m_buf = buf;

    in->Release();
  }
};

// register class
NCB_REGISTER_CLASS(PSBFile) {
  Constructor<tTJSVariant>(0);
  NCB_PROPERTY_RO(root, getRoot);
};
