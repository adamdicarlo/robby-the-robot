# Robby the Robot

Robby is a robot who tries to pick up as many littered soda cans as possible in a 10x10 (100-cell) office, which can have up to 1 can per cell. More about Robby is at http://adamdicarlo.com/genetic-algorithm-robby-the-robot (broken link, sorry).


## Variable naming convention

This codebase uses an uncommon variable naming convention. It makes sense--with an explanation--I swear!

The convention is something like Applications Hungarian. (It's not widely documented enough to know for sure.)

It's similar to classic Windows APIs (Systems Hungarian), where each variable's name has a prefix
indicating its type, like `w`, `dw`, `pb`: word, dword, pointer to a byte, respectively.

Instead of those low-level prefixes that strictly represent data type, we use these, which signify what the variable represents:

- `c`: count
- `i`: index into an array
- `r`: real number (floating point)
- `n`: number (generic)
- `min`: lowest value or index
- `max`: highest value or index
- `p`: pointer
- `rg`: array (rg as in "range")
- `sz`: standard C string (z for zero/nul-terminated)

These prefixes can be combined with each other and with higher-level data
types (structs), as well.

Prefixes for types specific to this program:

- `act`: ACTION struct
- `stg`: STRATEGY struct
- `wld`: WORLD struct
- `Pop`: POPULATION struct (capitalization to avoid leading-p confusion with
  pointers.)

Examples:

- `pstgMother` is a pointer to a STRATEGY for a "mother" (one of the
  parents used when "mating" strategies)
- `iact` is an index into an array of actions ([i]ndex and [act]ion are clear
  enough, thus it doesn't need to be `irgact`)
- `cStrategies` is a count of strategies. More traditional names for this
  variable, would be `num_strategies` or `strategy_count`.

