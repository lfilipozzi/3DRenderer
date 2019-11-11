#include "../include/abstractobject.h"

/***
 *              ____   _      _              _         
 *             / __ \ | |    (_)            | |        
 *            | |  | || |__   _   ___   ___ | |_       
 *            | |  | || '_ \ | | / _ \ / __|| __|      
 *            | |__| || |_) || ||  __/| (__ | |_       
 *      __  __ \____/ |_.__/ | | \___| \___| \__|      
 *     |  \/  |             _/ |                       
 *     | \  / |  __ _  _ __|__/__ _   __ _   ___  _ __ 
 *     | |\/| | / _` || '_ \  / _` | / _` | / _ \| '__|
 *     | |  | || (_| || | | || (_| || (_| ||  __/| |   
 *     |_|  |_| \__,_||_| |_| \__,_| \__, | \___||_|   
 *                                    __/ |            
 *                                   |___/             
 */

// Instantiate static member variables
ObjectManager::ObjectsMap ObjectManager::m_objects;


ABCObject * ObjectManager::loadObject(
    QString name, std::unique_ptr<ABCObject> object
) {
    // Check if a model with the same name has already been loaded
    ObjectsMap::iterator it = m_objects.find(name);
    if (it != m_objects.end()) {
        // The object has already been loaded
        return it->second.get();
    }
    // The object has not been loaded yet
    m_objects[name] = std::move(object);
    return m_objects[name].get();
}


ABCObject * ObjectManager::getObject(QString name) {
    ObjectsMap::iterator it(m_objects.find(name));
    if (it != m_objects.end())
        return it->second.get();
    else
        return nullptr;
}


void ObjectManager::initialize() {
    // Delete all textures
    for (
        ObjectsMap::iterator it = m_objects.begin(); it != m_objects.end(); it++
    ) {
        if (it->second != nullptr)
            it->second->initialize();
    }
}


void ObjectManager::cleanUp() {
    // Delete all textures
    for (
        ObjectsMap::iterator it = m_objects.begin(); it != m_objects.end(); it++
    ) {
        if (it->second != nullptr)
            it->second->cleanUp();
    }
}



