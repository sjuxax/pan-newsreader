/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Pan - A Newsreader for Gtk+
 * Copyright (C) 2002-2006  Charles Kerr <charles@rebelbase.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _HAVE_GPGDEFS_H
#define _HAVE_GPGDEFS_H

#include <gmime/gmime.h>
#include <map>
#include <vector>

#ifdef HAVE_GMIME_CRYPTO

namespace pan
{

  struct Signer
  {
    std::string name;
    std::string key_id;
    std::string fpr;
    std::string trust;
    std::string status;
    std::string error;

    bool ok;
    bool never_expires;
    time_t created, expires;

    Signer() : ok(false), never_expires(false), created((time_t)0), expires((time_t)0)  {}
  };

  struct GPGSignersInfo
  {
    std::vector<Signer> signers;
  };

  typedef std::map<std::string,GPGSignersInfo> signers_m;

  extern GMimeCryptoContext* gpg_ctx;
  extern bool gpg_inited;

  enum GPGDecType
  {
    GPG_VERIFY,
    GPG_DECODE
  };

  enum GPGEncType
  {
    GPG_ENCODE,
    GPG_SIGN,
    GPG_ENCODE_AND_SIGN
  };

  /* Error struct for gpg_decrypt */
  struct GPGDecErr
  {
    GError* err;
    std::string output;
    bool dec_ok;
    bool verify_ok;
    bool no_sigs;
    GPGDecType type;
    GPGSignersInfo signers;
    GMimeObject * decrypted;
    GMimeDecryptResult * result;

    GPGDecErr(GPGDecType t) : err(NULL), no_sigs(true), type(t), decrypted(NULL), result(g_mime_decrypt_result_new())  {}
    ~GPGDecErr()
    {
      if (err) g_error_free(err);
      err = NULL;
//        if (decrypted) g_object_unref(decrypted) ;
//      if (result) g_object_unref(result);
    }

    GPGDecErr() : err(NULL), no_sigs(true), type(GPG_DECODE), decrypted(NULL), result(g_mime_decrypt_result_new()) {}

    void clear()
    {
      if (err) g_error_free(err);
      err = NULL;
      signers.signers.clear();
      verify_ok = false;
      dec_ok = false;
      no_sigs = true;
    }

  };

  /* Error struct for gpg_sign_and_encrypt */
  struct GPGEncErr
  {
    GError* err;
    std::string output;
    bool enc_ok;
    bool sign_ok;
    bool no_sigs;
    GPGEncType type;
    GPGSignersInfo signers;

    GPGEncErr(GPGEncType t) : err(NULL), no_sigs(true), type(t)   {}
    ~GPGEncErr()
    {
      if (err) g_error_free(err);
      err = NULL;
    }

  };

  void init_gpg();
  void deinit_gpg();
  void fill_signer_info(GPGSignersInfo& info, GMimeSignatureList * sig_list);
  int
  __g_mime_multipart_signed_sign (GMimeMultipartSigned *mps, GMimeObject *content,
              GMimeCryptoContext *ctx, const char *userid,
              GMimeDigestAlgo digest, GError **err);

}

#endif // g_mime_crypto

#endif

