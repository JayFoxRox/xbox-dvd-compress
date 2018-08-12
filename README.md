# Warning

**These are proof of concepts, not functional tools.**

---

To build this project you need git and CMake:

```
git clone https://github.com/JayFoxRox/xbox-dvd-compress.git
cd xbox-dvd-compress
mkdir build
cd build
cmake ..
make
```

# Tools

**All of the tools expect your [DVD image in redump format](http://wiki.redump.org/index.php?title=Microsoft_Xbox_Dumping_Guide)).**

### get_version

This tool can be used to get information about Xbox the tools that were used to create the image.

Run it like this: `./get_version "Petit Copter (Japan).iso"`

The output will look like this:

```
xblayout version: 1.0.4361.1
xbpremaster version: 1.0.4361.2
```

### tag_sectors

This tool will first bruteforce the seed for the random stream which was used to generate padding.
Then it will scan over the image and trace the random stream.

Run it like this: `./tag_sectors "Petit Copter (Japan).iso"`

The output will look like this:

```
Starting bruteforce
Found seed 0xF401863E
Completed bruteforce (Success)
at offset: 405798912; sector 198144; data area sector: 0; random
at offset: 405800960; sector 198145; data area sector: 1; random
at offset: 405803008; sector 198146; data area sector: 2; random
at offset: 405805056; sector 198147; data area sector: 3; random
at offset: 405807104; sector 198148; data area sector: 4; random
at offset: 405809152; sector 198149; data area sector: 5; random
at offset: 405811200; sector 198150; data area sector: 6; random
at offset: 405813248; sector 198151; data area sector: 7; random
at offset: 405815296; sector 198152; data area sector: 8; random
at offset: 405817344; sector 198153; data area sector: 9; random
at offset: 405819392; sector 198154; data area sector: 10; random
at offset: 405821440; sector 198155; data area sector: 11; random
at offset: 405823488; sector 198156; data area sector: 12; random
at offset: 405825536; sector 198157; data area sector: 13; random
at offset: 405827584; sector 198158; data area sector: 14; random
at offset: 405829632; sector 198159; data area sector: 15; random
at offset: 405831680; sector 198160; data area sector: 16; random
at offset: 405833728; sector 198161; data area sector: 17; random
at offset: 405835776; sector 198162; data area sector: 18; random
at offset: 405837824; sector 198163; data area sector: 19; random
at offset: 405839872; sector 198164; data area sector: 20; random
at offset: 405841920; sector 198165; data area sector: 21; random
at offset: 405843968; sector 198166; data area sector: 22; random
at offset: 405846016; sector 198167; data area sector: 23; random
at offset: 405848064; sector 198168; data area sector: 24; random
at offset: 405850112; sector 198169; data area sector: 25; random
at offset: 405852160; sector 198170; data area sector: 26; random
at offset: 405854208; sector 198171; data area sector: 27; random
at offset: 405856256; sector 198172; data area sector: 28; random
at offset: 405858304; sector 198173; data area sector: 29; random
at offset: 405860352; sector 198174; data area sector: 30; random
at offset: 405862400; sector 198175; data area sector: 31; random
at offset: 405864448; sector 198176; data area sector: 32; unknown
at offset: 405866496; sector 198177; data area sector: 33; unknown
at offset: 405868544; sector 198178; data area sector: 34; random
...
at offset: 7825160192; sector 3820879; data area sector: 3622735; unknown
at offset: 7825162240; sector 3820880; data area sector: 3622736; unknown

Data-Area offset: 405798912 bytes
Random: 6761777152 bytes (+134217728 bytes security)
Repeated: 398854144 bytes
Unknown: 124514304 bytes

Total: 7825162240 bytes / 7825162240 bytes
```

Note that this tool does not support later images which used a RC4-drop-2048 random stream.

# Donate

If you like my work, a donation would be nice:

* [Patreon](https://www.patreon.com/jayfoxrox)
* [PayPal](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=x1f3o3x7x%40googlemail%2ecom&lc=GB&item_name=Jannik%20Vogel%20%28JayFoxRox%29&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHostedGuest)

Even a small amount does help me and shows appreciation. Thank you!

----

**(C) 2018 Jannik Vogel (JayFoxRox)**

All rights reserved.
