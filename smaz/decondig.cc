//
// Created by kiosk on 19-10-7.
//
#include "decondig.h"
#include <cstring>


std::string deconding::smaz_compress(const char *in, int inlen, std::string * out) {
    unsigned int h1,h2,h3=0;
    int verblen = 0;
    char verb[256];

    int out_offest = 0;

    while(inlen ) {

        int j = 7, needed;
        const char *slot;
        int flush = -1;
        h1 = h2 = in[0] << 3;
        if (inlen > 1) h2 += in[1];
        if (inlen > 2) h3 = h2 ^ in[2];
        if (j > inlen) j = inlen;

        /* Try to lookup substrings into the hash table, starting from the
         * longer to the shorter substrings */
        for (; j > 0; j--) {
            switch (j) {
                case 1:
                    slot = Smaz_cb[h1 % 241];
                    break;
                case 2:
                    slot = Smaz_cb[h2 % 241];
                    break;
                default:
                    slot = Smaz_cb[h3 % 241];
                    break;
            }
            while (slot[0]) {
                if (slot[0] == j && memcmp(slot + 1, in, j) == 0) {
                    /* Match found in the hash table,
                     * prepare a verbatim bytes flush if needed */
                    if (verblen) {
                        needed = (verblen == 1) ? 2 : 2 + verblen;
                        flush = out_offest;
                        out_offest += needed;
                    }
                    if(out_offest >= out->size())
                    {
                        out->resize(out_offest+1);
                    }
                    out->operator[](out_offest ) = slot[slot[0] + 1];
                    out_offest++;
                    inlen -= j;
                    in += j;
                    goto out;
                } else {
                    slot += slot[0] + 2;
                }
            }
        }
        /* Match not found - add the byte to the verbatim buffer */
        verb[verblen] = in[0];
        verblen++;
        inlen--;
        in++;
        out:
        /* Prepare a flush if we reached the flush length limit, and there
         * is not already a pending flush operation. */
        if ((flush == -1) && (verblen == 256 || (verblen > 0 && inlen == 0))) {
            needed = (verblen == 1) ? 2 : 2+verblen;
            flush = out_offest;
            out_offest += needed;
        }
        /* Perform a verbatim flush if needed */
        if (flush >= 0 ) {
            if(flush >= out->size())
            {
                out->resize(flush +1 );
            }
            if (verblen == 1) {
                out->operator[] (flush) = static_cast<signed char>(254);
                out->operator[] (flush+1 )= verb[0];
            } else {
                out->operator[] (flush)  = static_cast<signed char>(255);
                out->operator[] (flush+1 ) = static_cast<signed char>(verblen-1);
                if(flush + verblen > out->capacity())
                {
                    out->resize(flush + verblen + 1);
                }
                std::copy(verb,verb + verblen,out->begin() + flush + 2);
            }
            flush = -1;
            verblen = 0;
        }
   }
    return *out;
}
std::string deconding::smaz_decompress(const char *in, int inlen,std::string * out) {
    auto *c = (unsigned char*) in;
    int out_offest = 0;
    out->resize(inlen * 10);
    while(inlen) {
        if (*c == 254) {
            /* Verbatim byte */
            out->operator[](out_offest) = *(c+1);
            out_offest++;
            c += 2;
            inlen -= 2;
        } else if (*c == 255) {
            /* Verbatim string */
            int len = (*(c+1))+1;
         //   memcpy(out,c+2,len);
           std::copy(c + 2, c+ 2+ len,out->begin()+out_offest);
            out_offest += len;
            c += 2+len;
            inlen -= 2+len;
        } else {
            /* Codebook entry */
            const char *s = Smaz_rcb[*c];
            int len = strlen(s);
           // memcpy(out,s,len);
            std::copy(s, s+ len,out->begin()+out_offest);
            out_offest += len;
            c++;
            inlen--;
        }
    }
    out->shrink_to_fit();
    return * out;
}