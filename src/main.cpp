#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>

using namespace geode::prelude;

// Создаем правильную структуру полей для Geode v3
struct MyFields {
    CCLabelBMFont* m_bpmLabel = nullptr;
    CCSprite* m_heartSprite = nullptr;
    float m_updateTimer = 0.0f;
};

class $modify(MyPlayLayer, PlayLayer) {
    // Регистрируем структуру в классе игры
    std::unique_ptr<MyFields> m_myFields = std::make_unique<MyFields>();

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
        m_fields->m_myFields->m_heartSprite = heart;

        // Создаем текст для BPM
        auto label = CCLabelBMFont::create("80 BPM", "bigFont.fnt");
        label->setScale(0.4f);
        label->setAnchorPoint({0.0f, 0.5f});
        label->setPosition({75, 40});
        this->addChild(label, 100);
        m_fields->m_myFields->m_bpmLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto fields = m_fields->m_myFields.get();
        if (!fields->m_bpmLabel || !fields->m_heartSprite) return;

        // Таймер обновления пульса (раз в 0.7 секунды)
        fields->m_updateTimer += dt;
        if (fields->m_updateTimer >= 0.7f) {
            fields->m_updateTimer = 0.0f;

            float currentPercent = this->getCurrentPercent();

            // Читаем настройки из меню Geode
            auto endRange1 = Mod::get()->getSettingValue<int64_t>("range1-end");
            auto minBpm1 = Mod::get()->getSettingValue<int64_t>("range1-min-bpm");
            auto maxBpm1 = Mod::get()->getSettingValue<int64_t>("range1-max-bpm");
            auto minBpm2 = Mod::get()->getSettingValue<int64_t>("range2-min-bpm");
            auto maxBpm2 = Mod::get()->getSettingValue<int64_t>("range2-max-bpm");

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

            // Обновляем текст и делаем анимацию пульсации сердца
            fields->m_bpmLabel->setString(fmt::format("{} BPM", currentBPM).c_str());
            fields->m_heartSprite->setScale(0.75f);
            fields->m_heartSprite->runAction(CCScaleTo::create(0.2f, 0.6f));
        }
    }
};
