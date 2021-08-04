/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
 */

#if !defined(_usb_h_) || defined(__PRINTHEX_H__)
#error "Never include printhex.h directly; include Usb.h instead"
#else
#define __PRINTHEX_H__

void E_Notifyc(char c, int lvl);

template <class T>
void PrintHex(T val, int lvl) {
        int num_nibbles = sizeof (T) * 2;

        do {
                char v = 48 + (((val >> (num_nibbles - 1) * 4)) & 0x0f);
                if(v > 57) v += 7;
                E_Notifyc(v, lvl);
        } while(--num_nibbles);
}

template <class T> void D_PrintHex(T val __attribute__((unused)), int lvl __attribute__((unused))) {
}



#endif // __PRINTHEX_H__
