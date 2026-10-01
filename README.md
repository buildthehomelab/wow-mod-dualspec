# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## mod-dualspec
### This is a module for [AzerothCore](http://www.azerothcore.org)

# Module info

- Name: mod-dualspec
- License: GNU General Public License v3.0
- Fork of [Lichborne-AC/mod-dualspec](https://github.com/Lichborne-AC/mod-dualspec)
  with a configurable price and minimum level

# Description

Adds a single player chat command, `.dualspec`, which unlocks Dual Talent
Specialization for the calling character without visiting a class trainer.
The price (gold) and the minimum level are set in the config.

The unlock casts the same two spells as the core's class trainer gossip
(`GOSSIP_OPTION_LEARNDUALSPEC`): 63680 teaches the Activate Primary/Secondary
Spec spells, and 63624 raises the spec count to 2 through
`Player::UpdateSpecCount`, which saves it to the character DB.

# Usage

In-game, type:

```
.dualspec
```

With `DualSpec.Cost` above 0, `.dualspec` shows the price and
`.dualspec confirm` pays it and unlocks. With a cost of 0, `.dualspec` unlocks
straight away.

The command refuses when:

- the module is disabled in config
- the character already has two specs
- the character is below `DualSpec.MinLevel`
- the character is dead or in combat
- the character can't afford `DualSpec.Cost`

If the unlock fails for any reason, the money is refunded.

# Module integration

- Includes configuration (.conf)?: Yes, copied by CMake
- Includes SQL patches?: No
- Core hooks used:
    + CommandScript (registers `.dualspec`, `SEC_PLAYER`, in-game only)

# How to install

The repo has a `wow-` prefix; the module folder must not. AzerothCore derives
the loader name from the folder.

```bash
cd modules
git clone https://github.com/buildthehomelab/wow-mod-dualspec.git mod-dualspec
```

Then re-run CMake, rebuild, and `make install`.

# Configuration

Copy `mod_dualspec.conf.dist` to `mod_dualspec.conf` in your server's
`etc/modules/` directory and edit:

| Key | Default | Meaning |
| --- | --- | --- |
| `DualSpec.Enable` | `1` | `0` disables the command |
| `DualSpec.Cost` | `1000` | Price in gold; `0` makes it free |
| `DualSpec.MinLevel` | `10` | Minimum character level |

`DualSpec.MinLevel` is separate from the worldserver `MinDualSpecLevel`, which
only controls the class trainer option.

# Credits

* Original module: [Lichborne-AC/mod-dualspec](https://github.com/Lichborne-AC/mod-dualspec)
* AzerothCore: [repository](https://github.com/azerothcore) -
  [website](http://azerothcore.org/) -
  [discord](https://discord.gg/PaqQRkd)
