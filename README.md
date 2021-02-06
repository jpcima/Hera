# Hera

Juno emulation synthesizer

## About

This synthesizer is considered of **alpha quality** currently.  
It can produce some decent sounds, but the timbres can, and will most likely change in future revisions.  
It is known that the emulation is not accurate in many aspects.

The work is based partially on reverse-engineering and code made by others. See references [1] [2].  
Accordingly, the GPL license applies to this project.

## To-do items

Any contributions will be welcome.

- [ ] add the manual LFO trigger
- [ ] improve the VCF model; excessive self-oscillation
- [ ] add the arpeggiator
- [ ] envelope release has to go through decay first
- [ ] add preset management inside the editor, which includes selecting and renaming

## Instrutions

```
git clone --recursive https://github.com/jpcima/Hera.git
cd Hera
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

## References

- [1] https://github.com/pendragon-andyh/Juno60
- [2] https://github.com/pendragon-andyh/junox

## Credits

Thanks @pendragon-andyh, @dzannotti for their programming work and research.
