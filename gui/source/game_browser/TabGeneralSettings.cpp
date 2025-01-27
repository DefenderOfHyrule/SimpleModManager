//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include <TabGeneralSettings.h>
#include <GlobalObjects.h>

#include "GenericToolbox.h"
#include "Logger.h"

LoggerInit([]{
  Logger::setUserHeaderStr("[tab_general_settings]");
});

TabGeneralSettings::TabGeneralSettings() {
  LogInfo << __METHOD_NAME__ << std::endl;

  rebuild_layout();

}

void TabGeneralSettings::rebuild_layout() {

  itemCurrentInstallPreset = new brls::ListItem(
    "\uE255 Current Install Preset:",
    "Specify from which base folder mods will be installed.\n"\
    "- If you are using Atmosphere, mods have to be installed in /atmosphere/. "\
    "This corresponds to the \"default\" preset. You need to take this path into account in your mod tree structure.\n"\
    "- If you want to set a specific install folder for a given game, please refer to its Option tab and go to \"Attribute a config preset\".",
    ""
  );
  itemCurrentInstallPreset->setValue(GlobalObjects::getModBrowser().get_parameters_handler().get_current_config_preset_name());
  itemCurrentInstallPreset->getClickEvent()->subscribe([this](View* view) {
    brls::ValueSelectedEvent::Callback valueCallback = [this](int result) {
      if (result == -1)
        return;

      GlobalObjects::getModBrowser().change_config_preset(GlobalObjects::getModBrowser().get_parameters_handler().get_presets_list()[result]);
      LogDebug("Selected : %s -> %s",
               GlobalObjects::getModBrowser().get_parameters_handler().get_current_config_preset_name().c_str(),
               GlobalObjects::getModBrowser().get_mod_manager().get_install_mods_base_folder().c_str());
      this->itemCurrentInstallPreset->setValue(GlobalObjects::getModBrowser().get_parameters_handler().get_current_config_preset_name());
//       this->valueEvent.fire(result); // not now
    };
    auto presets_list_labels = GlobalObjects::getModBrowser().get_parameters_handler().get_presets_list();
    for(auto& preset_label : presets_list_labels){
      auto install_folder = GlobalObjects::getModBrowser().get_parameters_handler().get_parameter(preset_label + "-install-mods-base-folder");
      preset_label += " \uE090 \"" + install_folder + "\"";
    }
    brls::Dropdown::open(
      "Current Install Preset:",
      presets_list_labels,
      valueCallback,
      GlobalObjects::getModBrowser().get_parameters_handler().get_current_config_preset_id()
    );
  });
  this->addView(itemCurrentInstallPreset);

  GlobalObjects::getModBrowser().get_parameters_handler().get_presets_list();

  auto* itemStoredModsBaseFolder = new brls::ListItem(
    "\uE431 Stored Mods Location:",
    "This is the place where SimpleModManager will look for your mods. From this folder, the tree structure must look like this:\n"\
    "./<name-of-the-game-or-category>/<mod-name>/<mod-tree-structure>.",
    "");
  itemStoredModsBaseFolder->setValue(GlobalObjects::getModBrowser().get_parameters_handler().get_parameter("stored-mods-base-folder"));
  this->addView(itemStoredModsBaseFolder);

  auto* itemUseUI = new brls::ListItem("\uE072 Disable the GUI", "If you want to go back on the old UI, select this option.");

  itemUseUI->updateActionHint(brls::Key::B, "Back");
  itemUseUI->registerAction("Select", brls::Key::A, [](){

    auto* dialog = new brls::Dialog("Do you want to disable this GUI and switch back to the console-style UI ?");

    dialog->addButton("Yes", [](brls::View* view) {
      GlobalObjects::getModBrowser().get_parameters_handler().set_parameter("use-gui", "0");
      brls::Application::quit();
//      GlobalObjects::setTriggerSwitchUI(true);
//      GlobalObjects::disable_cout_redirection();
    });
    dialog->addButton("No", [dialog](brls::View* view) {
      dialog->close();
    });

    dialog->setCancelable(true);
    dialog->open();

    return true;
  });
  this->addView(itemUseUI);


}
