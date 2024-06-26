/************************************************************************************
 *
 * Copyright 1993,2001,2023 Craig Edwards <brain@ssod.org>
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
#include <dpp/dpp.h>
#include <fmt/format.h>
#include <rps/domain/lang.h>
#include <rps/domain/rps.h>
#include <sys/stat.h>

namespace i18n {

std::shared_mutex lang_mutex;
static dpp::interaction_create_t english{};
time_t last_lang{0};
json *lang{nullptr};

time_t get_mtime(const char *path) {
  struct stat stat_buf {};
  if (stat(path, &stat_buf) == -1) {
    return 0;
  }
  return stat_buf.st_mtime;
}

void check_lang_reload(dpp::cluster &bot) {
  if (get_mtime("lang.json") > last_lang) {
    std::unique_lock lang_lock(lang_mutex);
    last_lang = get_mtime("lang.json");
    std::ifstream langfile("lang.json");
    json *new_lang = new json();
    try {
      json *old_lang = lang;

      // Parse updated contents
      langfile >> *new_lang;

      lang = new_lang;
      delete old_lang;
    } catch (const std::exception &e) {
      bot.log(dpp::ll_error, fmt::format("Error in lang.json: ", e.what()));
      delete new_lang;
    }
  }
}

void load_lang(dpp::cluster &bot) {
  std::unique_lock lang_lock(lang_mutex);
  last_lang = get_mtime("lang.json");
  english.command.locale = "en";
  std::ifstream lang_file("lang.json");
  lang = new json();
  lang_file >> *lang;
  bot.log(dpp::ll_info,
          fmt::format("Language strings count: {}", lang->size()));
}

std::string tr(const std::string &k,
               const dpp::interaction_create_t &interaction) {
  std::string lang_name = (interaction.command.locale == "")
                              ? "en"
                              : interaction.command.locale.substr(0, 2);
  std::shared_lock lang_lock(lang_mutex);
  try {
    auto o = lang->find(k);
    if (o != lang->end()) {
      auto v = o->find(lang_name);
      if (v != o->end()) {
        return v->get<std::string>();
      }
      return tr(k, english);
    }
  } catch (const std::exception &e) {
    std::cout << "i18n error on " << k << " " << lang_name << ": " << e.what()
              << "\n";
  }
  return k;
}

std::string discord_lang(const std::string &l) {
  if (l == "es") {
    return "es-ES";
  } else if (l == "pt") {
    return "pt-BR";
  } else if (l == "sv") {
    return "sv-SE";
  } else if (l == "zh") {
    return "zh-CN";
  }
  return l;
};

dpp::command_option_choice tr(dpp::command_option_choice choice) {
  auto o = lang->find(choice.name);
  if (o != lang->end()) {
    dpp::interaction_create_t e{};
    for (auto v = o->begin(); v != o->end(); ++v) {
      if (v.key() == "en") {
        continue;
      }
      /* Note: We don't translate the value for choice, this remains constant
       * internally */
      e.command.locale = discord_lang(v.key());
      choice.add_localization(e.command.locale, tr(choice.name, e));
    }
    choice.name = tr(choice.name, english);
  }
  return choice;
}

dpp::command_option tr(dpp::command_option opt) {
  auto o = lang->find(opt.name);
  if (o != lang->end()) {
    dpp::interaction_create_t e{};
    for (auto v = o->begin(); v != o->end(); ++v) {
      if (v.key() == "en") {
        continue;
      }
      e.command.locale = discord_lang(v.key());
      opt.add_localization(e.command.locale, tr(opt.name, e),
                           tr(opt.description, e));
    }
    opt.name = tr(opt.name, english);
    opt.description = tr(opt.description, english);
  }
  for (auto &choice : opt.choices) {
    choice = tr(choice);
  }
  for (auto &option : opt.options) {
    option = tr(option);
  }
  return opt;
}

dpp::slashcommand tr(dpp::slashcommand cmd) {
  auto o = lang->find(cmd.name);
  if (o != lang->end()) {
    dpp::interaction_create_t e{};
    for (auto v = o->begin(); v != o->end(); ++v) {
      if (v.key() == "en") {
        continue;
      }
      e.command.locale = discord_lang(v.key());
      cmd.add_localization(e.command.locale, tr(cmd.name, e),
                           tr(cmd.description, e));
    }
    cmd.name = tr(cmd.name, english);
    cmd.description = tr(cmd.description, english);
  }
  for (auto &option : cmd.options) {
    option = tr(option);
  }
  return cmd;
}

}; // namespace i18n