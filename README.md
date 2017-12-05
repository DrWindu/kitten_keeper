# Kitten Keeper

A kitten managment game for LD40 !

## Presentation

Kittens are so cute ! You cannot have to many of them. Or can you ?

You have to take care of kittens. Give them food, a place to sleep, etc. and soon you will have plenty of them. What could possibly go wrong ?

## Compilation

Clone the repository. We use a submodule to include our "game engine", Lair. So you should clone with `--recursive`:
```
git clone --recursive <clone-url>
```

The only dependency of the project is [Lair](https://github.com/draklaw/lair), our homebrew game engine. However, Lair has several dependencies. Refer to Lair's `Readme.md` for more information. Using a package manager is highly recommended. We build the Windows version using MSYS2.

We use cmake to compile. Once the dependencies are installed, assuming they are in a standard place where cmake can find them, all you need to do is
```
mkdir <some-build-directory>
cd <some-build-directory>
cmake <project-root>
make
```

If, as suggested above, you choose to do an out-of-source build, you must make sure that the game can find the assets folder. Just copy or link the asset folder in the directory of the executable, and you're good to go. If the game complain about missing DLLs (typical under Windows), you have to copy them to the executable directory. Now enjoy the game !
