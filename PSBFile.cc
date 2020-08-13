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

class PSBFile {
public:
  PSBFile(const tTJSVariant &filename)
      : m_psb(nullptr), m_buf(nullptr), m_root() {
    // load the file and load into a buffer
    open(filename);

    // load into psb_t
    m_psb = new psb_t(m_buf);

    // TODO: translate into tTJSVariant
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
