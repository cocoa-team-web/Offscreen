#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>
#include <unordered_set>

using namespace geode::prelude;

class $modify(CullPlayLayer, PlayLayer) {
    struct Fields {
        int m_tick = 0;
        // объекты, которые скрыли именно мы (чтобы не трогать то, что скрыла сама игра)
        std::unordered_set<GameObject*> m_hidden;
    };

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("enabled")) return;

        auto f = m_fields.self();
        int interval = static_cast<int>(mod->getSettingValue<int64_t>("interval"));
        if (++f->m_tick < interval) return;
        f->m_tick = 0;

        if (!m_objects || !m_objectLayer) return;

        float margin = static_cast<float>(mod->getSettingValue<int64_t>("margin"));

        // Видимая область в координатах уровня.
        // Берём все 4 угла экрана, чтобы работало при повороте/зуме камеры.
        auto win = CCDirector::get()->getWinSize();
        CCPoint corners[4] = {
            {0.f, 0.f}, {win.width, 0.f},
            {0.f, win.height}, {win.width, win.height}
        };

        float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
        for (auto const& c : corners) {
            auto p = m_objectLayer->convertToNodeSpace(c);
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }
        minX -= margin; minY -= margin;
        maxX += margin; maxY += margin;

        // Скрываем только отрисовку (setVisible).
        // Объект остаётся в игре: коллизии, триггеры, группы, движение работают как раньше.
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            if (!obj) continue;

            auto pos = obj->getPosition();
            bool inside = pos.x >= minX && pos.x <= maxX &&
                          pos.y >= minY && pos.y <= maxY;

            if (inside) {
                if (f->m_hidden.erase(obj)) obj->setVisible(true);
            } else if (obj->isVisible()) {
                obj->setVisible(false);
                f->m_hidden.insert(obj);
            }
        }
    }
};
