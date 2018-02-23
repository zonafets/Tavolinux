set $1 ../server
rm -f cmdpaycard.hpp
rm -f cmdlangtoken.hpp
rm -f cmddiritti.hpp
rm -f cmdcodicigruppi.hpp
rm -f tavolo_bits.hpp
rm -f tavolo_tipi.hpp
rm -f moduli.hpp
ln -s $1/cmdpaycard.hpp .
ln -s $1/cmdlangtoken.hpp .
ln -s $1/cmddiritti.hpp .
ln -s $1/cmdcodicigruppi.hpp .
ln -s $1/tavolo_bits.hpp .
ln -s $1/tavolo_tipi.hpp .
ln -s $1/moduli.hpp .
set $1
