/*
    Copyright 2011 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "AndroidApplication.hpp"
#include <iostream>
#include <cstdlib>
#include <gdx-cpp/Graphics.hpp>
#include <gdx-cpp/Gdx.hpp>
#include <gdx-cpp/implementation/System.hpp>

using namespace gdx_cpp::backends::nix;
using namespace gdx_cpp;

gdx_cpp::backends::nix::AndroidApplication::AndroidApplication(gdx_cpp::ApplicationListener* listener,
                                                           const std::string& title, int width, int height,
                                                           bool useGL20IfAvailable)
:  Synchronizable(Gdx::system->getMutexFactory())
    , width(width)
    , height(height)
    , title(title)
    , useGL20iFAvailable(useGL20IfAvailable)
    , listener(listener)
    , graphics(0)
    , input(0)
    , logLevel(gdx_cpp::Application::LOG_INFO)
{
    initialize();
}

void AndroidApplication::initialize() {
        graphics = new AndroidGraphics();
        input = new AndroidInput();
        
        graphics->initialize();
        graphics->setTitle(this->title);
        graphics->setDisplayMode(width, height, false);

        Gdx::initialize(this, graphics, NULL, NULL, NULL);
        
        this->run();
}

void backends::nix::AndroidApplication::onRunnableStop()
{
    //DUMMY
}

void backends::nix::AndroidApplication::run()
{
    listener->create();
    listener->resize(graphics->getWidth(), graphics->getHeight());
    
    while (true) {
        graphics->updateTime();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->exit();
                return;
            } else {
                this->input->processEvents(event);
            }
        }        
        
        {
            lock_holder hnd = synchronize();

            std::list < Runnable::ptr >::iterator it = runnables.begin();
            std::list < Runnable::ptr >::iterator end = runnables.end();

            for(;it != end; ++it) {
                (*it)->run();
            }

            runnables.clear();
        }
        
        listener->render();
        graphics->update();
    }
}


std::ostream& AndroidApplication::error(const std::string& tag)
{
    std::cerr << "LIBGDX-CPP: " << tag;
    return std::cerr;
}

void gdx_cpp::backends::nix::AndroidApplication::exit()
{
    ::exit(0);
}

Audio* gdx_cpp::backends::nix::AndroidApplication::getAudio()
{

}

Files* gdx_cpp::backends::nix::AndroidApplication::getFiles()
{

}

Graphics* gdx_cpp::backends::nix::AndroidApplication::getGraphics()
{
    return graphics;
}

Input* gdx_cpp::backends::nix::AndroidApplication::getInput()
{
    return input;
}

Preferences* gdx_cpp::backends::nix::AndroidApplication::getPreferences(std::string& name)
{

}

gdx_cpp::Application::ApplicationType gdx_cpp::backends::nix::AndroidApplication::getType()
{
    return gdx_cpp::Application::Desktop;
}

std::ostream& gdx_cpp::backends::nix::AndroidApplication::log(const std::string& tag)
{
    if (logLevel == gdx_cpp::Application::LOG_NONE)
        return std::cout;
    
    std::cout << tag << ": ";
    return std::cout;
}

int gdx_cpp::backends::nix::AndroidApplication::getVersion()
{
    return 0.1;
}

void gdx_cpp::backends::nix::AndroidApplication::postRunnable(Runnable::ptr runnable)
{
    lock_holder hnd = synchronize();
    runnables.push_back(runnable);
}

void gdx_cpp::backends::nix::AndroidApplication::setLogLevel(int logLevel)
{
    logLevel = logLevel;
}
