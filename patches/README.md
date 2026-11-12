# Patches

`gp2040-ce-keyboard-left-stick.patch` is a patch against GP2040-CE v0.7.12-117 (commit `069d3108216c50f704601177e7cf850760ece68d`). It changes the Keyboard/Mouse Host add-on:

- four new keys for the **left analog stick** (default W / S / A / D), editable in the add-on page,
- the mouse is converted to a stick as a smoothed velocity instead of a one-shot deflection, with a response curve and a **Dead Zone Offset** slider (default 20%),
- full deflection at 500 / sensitivity mouse counts per millisecond (10 at the default 50%).

Apply it and build as usual for your board:

```sh
git -C /path/to/GP2040-CE apply /path/to/seriouspuck/patches/gp2040-ce-keyboard-left-stick.patch
```

Existing GP2040-CE configs get the new defaults on first boot, no reset needed. Built `.uf2` files are not stored in git (see `.gitignore`); publish them as release assets instead.
