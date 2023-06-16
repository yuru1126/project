#include "Graphics/Graphics.h"
#include "ResourceManager.h"

std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
    //ƒ‚ƒfƒ‹‚ðŒŸõ
    for (auto itr = models.begin(); itr != models.end(); ++itr)
    {
        if (itr->first == filename)
        {
            return itr->second.lock();
        }
    }
    std::shared_ptr<ModelResource> n = std::make_shared<ModelResource>();
    n->Load(Graphics::Instance().GetDevice(), filename);
    models.insert(std::make_pair(filename, n));
    return n;
}