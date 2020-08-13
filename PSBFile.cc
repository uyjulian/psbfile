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

// #define DUMP_VALUE

// use for debugging
static void dump_psb_value(psb_t *psb, psb_value_t const *value, int indent = 0,
                           bool terminate_line = true);

static tTJSVariable convert_psb_value(psb_t *psb, psb_value_t const* value);

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

static void convert_psb_value(psb_t *psb, psb_value_t const *value) {
  static std::vector<std::string> type_map{
      "psb_value_t",    "psb_null_t",    "psb_boolean_t",
      "psb_resource_t", "psb_number_t",  "psb_array_t",
      "psb_string_t",   "psb_objects_t", "psb_collection_t",
  };

  auto res =
      std::find(type_map.begin(), type_map.end(), value->get_type_string());

  switch (std::distance(type_map.begin(), res)) {
  case 1:
    return tTJSVariable();
  case 2: {
    psb_boolean_t const *v = dynamic_cast<psb_boolean_t const *>(value);
    if (!v) {
      // TODO:
    }

    return tTJSVariable(v->get_boolean());
  }
  case 3:
    // TODO: octet?
    break;
  case 4: {
    psb_number_t const *v = dynamic_cast<psb_number_t const *>(value);
    if (!v) {
      // TODO:
    }

    switch (v->get_number_type()) {
    case 0:
      return tTJSVariable(v->get_integer());
      break;
    case 1:
      return tTJSVariable(v->get_float());
      break;
    case 2:
      return tTJSVariable(v->get_double());
      break;
    default:
      // TOOD:
      break;
    }
    break;
  }
  case 5:
    std::cout << "[";
    {
      psb_array_t const *v = dynamic_cast<psb_array_t const *>(value);
      if (!v) {
        // TODO:
      }

      // TODO:
    break;
  case 6: {
    psb_string_t const *v = dynamic_cast<psb_string_t const *>(value);
    if (!v) {
      // TODO:
    }

    // TODO:

    break;
  }
  case 7: {
    psb_objects_t const *v = dynamic_cast<psb_objects_t const *>(value);
    if (!v) {
      // TODO:
    }

    // TODO:

    break;
  }
  case 8: {
    psb_collection_t const *v = dynamic_cast<psb_collection_t const *>(value);
    if (!v) {
      // TODO:
    }

    // TODO:

    break;
  }
  case 0:
  default:
    // <invalid-type>
    break;
  }

  return tTJSVariable();
}

class PSBFile {
public:
  PSBFile(const tTJSVariant &filename)
      : m_psb(nullptr), m_buf(nullptr), m_root() {
    // load the file and load into a buffer
    open(filename);

    // load into psb_t
    m_psb = new psb_t(m_buf);

    // TODO: translate into tTJSVariant
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

  tTJSVariant getRoot() {
    // TODO: implement
    return m_root;
  }

private:
  psb_t *     m_psb;
  uint8_t *   m_buf;
  tTJSVariant m_root;

  void open(const tTJSVariant &filename) {

    auto in = TVPCreateIStream(filename, TJS_BS_READ);

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
