/************************************************************************************
 *
 * Copyright 2004 Craig Edwards <support@brainbox.cc>
 *
 * Core based on Sporks, the Learning Discord Bot, Craig Edwards (c) 2019.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/

#include "commands.h"
#include "embed.h"
#include "rps.h"
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <sporks/modules.h>
#include <sporks/statusfield.h>
#include <sporks/stringops.h>
#include <string>
#include <utility>

command_leave_t::command_leave_t(class RPSModule *_creator,
                                 const std::string &_base_command, bool adm,
                                 const std::string &descr,
                                 std::vector<dpp::command_option> options)
    : command_t(_creator, _base_command, adm, descr, std::move(options)) {}

void command_leave_t::call(const in_cmd &cmd, std::stringstream &tokens,
                           guild_settings_t &settings,
                           const std::string &username, bool is_moderator,
                           dpp::channel *c, dpp::user *user) {

  /* TODO: Free up game */
  creator->SimpleEmbed(cmd.interaction_token, cmd.command_id, settings, "",
                       fmt::format("**{}** has left.", username),
                       cmd.channel_id, "0 players are in the queue", "",
                       cmd.user.get_avatar_url(avatar_size));
}
