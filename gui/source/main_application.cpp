//
// Created by Adrien BLANCHET on 20/06/2020.
//

#include <main_application.h>
#include <borealis.hpp>
#include <GlobalObjects.h>
#include <toolbox.h>

#include <cstring>

main_application::main_application() {

  _current_state_ = GAME_BROWSER;
  _rootFrame_ = nullptr;
  _browser_list_ = nullptr;
  brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
  if (not brls::Application::init("SimpleModManager")){
    brls::Logger::error("Unable to init Borealis application");
    exit(EXIT_FAILURE);
  }
  reset();

}

main_application::~main_application() {

  reset();

}

void main_application::reset() {

  _is_initialized_ = false;
  if(_rootFrame_ == nullptr) _rootFrame_ = new brls::TabFrame(); // memory is handled by brls

}

void main_application::initialize() {

  if(not _is_initialized_){
    initialize_layout();

    // Add the root view to the stack
    brls::Application::pushView(_rootFrame_);

    _is_initialized_ = true;
  }

}

void main_application::initialize_layout(){

  _rootFrame_->setTitle("SimpleModManager");
  _rootFrame_->setFooterText(toolbox::get_app_version());
  _rootFrame_->setIcon("romfs:/images/icon.jpg");

  fill_browser_list();

  brls::List* testList = new brls::List();

  brls::ListItem* dialogItem = new brls::ListItem("Open a dialog");
  dialogItem->getClickEvent()->subscribe([](brls::View* view) {
    brls::Dialog* dialog = new brls::Dialog("Warning: PozzNX will wipe all data on your Switch and render it inoperable, do you want to proceed?");

    brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
      dialog->close();
      brls::Application::notify("Running PozzNX...");
    };

    dialog->addButton("Continue", closeCallback);
    dialog->addButton("Continue", closeCallback);
    dialog->addButton("Continue", closeCallback);

    dialog->setCancelable(false);

    dialog->open();
  });

  brls::ListItem* themeItem = new brls::ListItem("TV Resolution");
  themeItem->setValue("Automatic");

  brls::SelectListItem* jankItem = new brls::SelectListItem(
    "User Interface Jank",
    { "Native", "Minimal", "Regular", "Maximum", "SX OS", "Windows Vista", "iOS 14" });

  brls::ListItem* crashItem = new brls::ListItem("Divide by 0", "Can the Switch do it?");
  crashItem->getClickEvent()->subscribe([](brls::View* view) { brls::Application::crash("The software was closed because an error occured:\nSIGABRT (signal 6)"); });

  brls::ListItem* popupItem = new brls::ListItem("Open popup");
  popupItem->getClickEvent()->subscribe([](brls::View* view) {
    brls::TabFrame* popupTabFrame = new brls::TabFrame();
    popupTabFrame->addTab("Red", new brls::Rectangle(nvgRGB(255, 0, 0)));
    popupTabFrame->addTab("Green", new brls::Rectangle(nvgRGB(0, 255, 0)));
    popupTabFrame->addTab("Blue", new brls::Rectangle(nvgRGB(0, 0, 255)));
    brls::PopupFrame::open("Popup title", BOREALIS_ASSET("icon/borealis.jpg"), popupTabFrame, "Subtitle left", "Subtitle right");
  });

  brls::SelectListItem* layerSelectItem = new brls::SelectListItem("Select Layer", { "Layer 1", "Layer 2" });

  testList->addView(dialogItem);
  testList->addView(themeItem);
  testList->addView(jankItem);
  testList->addView(crashItem);
  testList->addView(popupItem);

  brls::Label* testLabel = new brls::Label(brls::LabelStyle::REGULAR, "For more information about how to use Nintendo Switch and its features, please refer to the Nintendo Support Website on your smart device or PC.", true);
  testList->addView(testLabel);

  brls::ListItem* actionTestItem = new brls::ListItem("Custom Actions");
  actionTestItem->registerAction("Show notification", brls::Key::L, [] {
    brls::Application::notify("Custom Action triggered");
    return true;
  });
  testList->addView(actionTestItem);

  brls::LayerView* testLayers = new brls::LayerView();
  brls::List* layerList1      = new brls::List();
  brls::List* layerList2      = new brls::List();

  layerList1->addView(new brls::Header("Layer 1", false));
  layerList1->addView(new brls::ListItem("Item 1"));
  layerList1->addView(new brls::ListItem("Item 2"));
  layerList1->addView(new brls::ListItem("Item 3"));

  layerList2->addView(new brls::Header("Layer 2", false));
  layerList2->addView(new brls::ListItem("Item 1"));
  layerList2->addView(new brls::ListItem("Item 2"));
  layerList2->addView(new brls::ListItem("Item 3"));

  testLayers->addLayer(layerList1);
  testLayers->addLayer(layerList2);

  layerSelectItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
    testLayers->changeLayer(selection);
  });

  testList->addView(layerSelectItem);

  _rootFrame_->addTab("Mods Browser", _browser_list_);
  _rootFrame_->addSeparator();
  _rootFrame_->addTab("Example", testList);
  _rootFrame_->addTab("Settings", testLayers);
  _rootFrame_->addTab("About", new brls::Rectangle(nvgRGB(255, 0, 0)));

}

void main_application::start_loop() {

  if(not _is_initialized_){
    return;
  }

  // Run the app
//  u64 kDown, kHeld;
  while (brls::Application::mainLoop()){

//    kDown = hidKeysDown(CONTROLLER_P1_AUTO);

//    if(kDown & KEY_B){
//      if(_rootFrame_->getSidebar()->isFocused()){
//
//        break;
//      }
//      brls::Logger::debug("KEY_B");
//    }

//    brls::Application::getCurrentFocus() == brls::Application::get
  }

}

void main_application::fill_browser_list() {

  if(_browser_list_ != nullptr){
    _browser_list_->willDisappear(true);
    delete _browser_list_;
  }

  _browser_list_ = new brls::List();

  auto mod_folders_list = GlobalObjects::get_mod_browser().get_selector().get_selection_list();
  for (int i_folder = 0; i_folder < int(mod_folders_list.size()); i_folder++) {

    std::string selected_folder = mod_folders_list[i_folder];
    auto mods_list = toolbox::get_list_of_subfolders_in_folder( GlobalObjects::get_mod_browser().get_current_directory() + "/" + selected_folder);
    int nb_mods = mods_list.size();
    auto* item = new brls::ListItem(selected_folder, "", std::to_string(nb_mods) + " mod(s) available.");
    item->setValue(GlobalObjects::get_mod_browser().get_selector().get_tag(i_folder));
    auto* icon = GlobalObjects::get_mod_browser().get_folder_icon(selected_folder);
    if(icon != nullptr){
      item->setThumbnail(icon, 0x20000);
    }
    item->registerAction("Open", brls::Key::A, [this, selected_folder]{
      brls::Logger::debug("Openning %s", selected_folder.c_str());
      return true;
    });
    _browser_list_->addView(item);

  }

}

main_application::BrowserState main_application::getCurrentState() const {
  return _current_state_;
}

void main_application::setCurrentState(main_application::BrowserState currentState) {
  _current_state_ = currentState;
}
