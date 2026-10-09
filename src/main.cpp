#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    // 1. ПРАВИЛЬНОЕ ОБЪЯВЛЕНИЕ ПОЛЕЙ ДЛЯ GEODE V3
    struct Fields {
        CCLabelBMFont* m_bpmLabel = nullptr;
        CCSprite* m_heartSprite = nullptr;
        float m_updateTimer = 0.0f;
    };

    bool init(GJGameLevel* level, bool usePracticeMode, bool isPlaytest) {
        if (!PlayLayer::init(level, usePracticeMode, isPlaytest)) return false;

        // Создаем иконку сердца
        auto heart = CCSprite::createWithSpriteFrameName("GJ_heart.png");
        if (!heart) {
            heart = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        }
        
        heart->setColor({255, 50, 50});
        heart->setScale(0.6f);
        heart->setPosition({60, 40}); // Левый нижний угол
        this->addChild(heart, 100);
        
        // 2. ПРАВИЛЬНОЕ ПРИСВОЕНИЕ ЧЕРЕЗ МАКРОС M_FIELDS
        m_fields->m_heartSprite = heart;

        // Создаем текст для BPM
        auto label = CCLabelBMFont::create("80 BPM", "bigFont.fnt");
        label->setScale(0.4f);
        label->setAnchorPoint({0.0f, 0.5f});
        label->setPosition({75, 40});
        this->addChild(label, 100);
        
        m_fields->m_bpmLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        if (!m_fields->m_bpmLabel || !m_fields->m_heartSprite) return;

        // Таймер обновления пульса (раз в 0.7 секунды)
        m_fields->m_updateTimer += dt;
        if (m_fields->m_updateTimer >= 0.7f) {
            m_fields->m_updateTimer = 0.0f;

            float currentPercent = this->getCurrentPercent();

            // Читаем настройки из меню Geode с правильным приведением типов
            int endRange1 = static_cast<int>(Mod::get()->getSettingValue<int64_t>("range1-end"));
            int minBpm1 = static_cast<int>(Mod::get()->getSettingValue<int64_t>("range1-min-bpm"));
            int maxBpm1 = static_cast<int>(Mod::get()->getSettingValue<int64_t>("range1-max-bpm"));
            int minBpm2 = static_cast<int>(Mod::get()->getSettingValue<int64_t>("range2-min-bpm"));
            int maxBpm2 = static_cast<int>(Mod::get()->getSettingValue<int64_t>("range2-max-bpm"));

            int targetMin = minBpm1;
            int targetMax = maxBpm1;

            if (currentPercent > endRange1) {
                float progress = (currentPercent - endRange1) / (100.0f - endRange1);
                targetMin = static_cast<int>(minBpm1 + (minBpm2 - minBpm1) * progress);
                targetMax = static_cast<int>(maxBpm1 + (maxBpm2 - maxBpm1) * progress);
            }

            // Рандомим текущий пульс
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(targetMin, targetMax);
            int currentBPM = distr(gen);

            // 5. ИСПРАВЛЕННЫЙ ВЫВОД СТРОКИ БЕЗ СЛОМАННОГО FMT
            std::string bpmStr = std::to_string(currentBPM) + " BPM";
            m_fields->m_bpmLabel->setString(bpmStr.c_str());
            
            m_fields->m_heartSprite->setScale(0.75f);
            m_fields->m_heartSprite->runAction(CCScaleTo::create(0.2f, 0.6f));
        }
    }
};
