/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under
 * GNU General Public License v3.0: https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Adds the .dualspec player command, which unlocks Dual Talent
 * Specialization for the calling character. Uses the same unlock pathway
 * as the core's class trainer gossip (GOSSIP_OPTION_LEARNDUALSPEC).
 */

#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Player.h"

#include <algorithm>

using namespace Acore::ChatCommands;

namespace
{
    // Teaches the Activate Primary/Secondary Spec spells (63644, 63645).
    constexpr uint32 SPELL_TEACH_SPEC_SWITCHES = 63680;
    // SPELL_EFFECT_TALENT_SPEC_COUNT: Player::UpdateSpecCount(2).
    constexpr uint32 SPELL_LEARN_SECOND_SPEC   = 63624;

    // DualSpec.Cost is in gold; the player's money is in copper.
    uint32 GetCostCopper()
    {
        uint64 copper = uint64(sConfigMgr->GetOption<uint32>("DualSpec.Cost", 1000)) * GOLD;
        return uint32(std::min<uint64>(copper, MAX_MONEY_AMOUNT));
    }

    std::string FormatMoney(uint32 copper)
    {
        uint32 gold   = copper / GOLD;
        uint32 silver = (copper % GOLD) / SILVER;
        uint32 bronze = copper % SILVER;

        std::string out;
        if (gold)
            out += Acore::StringFormat("{}g", gold);
        if (silver)
            out += Acore::StringFormat("{}{}s", out.empty() ? "" : " ", silver);
        if (bronze || out.empty())
            out += Acore::StringFormat("{}{}c", out.empty() ? "" : " ", bronze);
        return out;
    }
}

class dualspec_commandscript : public CommandScript
{
public:
    dualspec_commandscript() : CommandScript("dualspec_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable dualspecCommandTable =
        {
            { "dualspec", HandleDualspecCommand, SEC_PLAYER, Console::No },
        };
        return dualspecCommandTable;
    }

    static bool HandleDualspecCommand(ChatHandler* handler, Optional<std::string> confirm)
    {
        if (!sConfigMgr->GetOption<bool>("DualSpec.Enable", true))
        {
            handler->SendSysMessage("|cffFF0000The .dualspec command is currently disabled on this server.|r");
            return true;
        }

        Player* player = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        if (!player)
            return false;

        if (player->GetSpecsCount() >= 2)
        {
            handler->SendSysMessage("|cffFFFF00You already have Dual Talent Specialization unlocked.|r");
            return true;
        }

        uint32 minLevel = sConfigMgr->GetOption<uint32>("DualSpec.MinLevel", 10);
        if (player->GetLevel() < minLevel)
        {
            handler->PSendSysMessage("|cffFF0000You must be at least level {} to use .dualspec.|r", minLevel);
            return true;
        }

        if (!player->IsAlive())
        {
            handler->SendSysMessage("|cffFF0000You cannot use .dualspec while dead.|r");
            return true;
        }

        if (player->IsInCombat())
        {
            handler->SendSysMessage("|cffFF0000You cannot use .dualspec while in combat.|r");
            return true;
        }

        uint32 cost = GetCostCopper();
        if (cost && !player->HasEnoughMoney(cost))
        {
            handler->PSendSysMessage("|cffFF0000Dual Talent Specialization costs {}. You don't have enough money.|r", FormatMoney(cost));
            return true;
        }

        // Charging money from a chat command needs an explicit second step.
        if (cost && (!confirm || !StringEqualI(*confirm, "confirm")))
        {
            handler->PSendSysMessage("|cffFFFF00Dual Talent Specialization costs {}. Type |cffFFFFFF.dualspec confirm|cffFFFF00 to buy it.|r", FormatMoney(cost));
            return true;
        }

        if (cost)
            player->ModifyMoney(-int32(cost));

        // Same casts as the trainer gossip; both target self and must be cast by the player.
        player->CastSpell(player, SPELL_TEACH_SPEC_SWITCHES, true, nullptr, nullptr, player->GetGUID());
        player->CastSpell(player, SPELL_LEARN_SECOND_SPEC, true, nullptr, nullptr, player->GetGUID());

        // Safety net in case a spell was blocked: the spec count is what matters.
        if (player->GetSpecsCount() < 2)
        {
            if (cost)
                player->ModifyMoney(int32(cost));
            handler->SendSysMessage("|cffFF0000Dual Talent Specialization could not be unlocked. Nothing was charged.|r");
            return true;
        }

        handler->SendSysMessage("|cff4CFF00Dual Talent Specialization unlocked!|r");
        return true;
    }
};

void AddDualSpecCommandScripts()
{
    new dualspec_commandscript();
}
