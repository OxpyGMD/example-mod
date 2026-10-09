#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <random>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    CCLabelBMFont* m_bpmLabel = nullptr;
    CCSprite* m_heartSprite = nullptr;
    float m_updateTimer = 0.0f;

    bool init(GJGameLevel* level, bool usePracticeMode, bool isPlaytest) {
        if (!PlayLayer::init(level, usePracticeMode, isPlaytest)) return false;

        // Creating the heart icon texture
        m_fields->m_heartSprite = CCSprite::createWithSpriteFrameName("GJ_heart.png");
        if (!m_fields->m_heartSprite) {
            m_fields->m_heartSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        }
        
        // Coloring the heart red
        m_fields->m_heartSprite->setColor({255, 50, 50});
        m_fields->m_heartSprite->setScale(0.6f);
        m_fields->m_heartSprite->setPosition({60, 40}); // Bottom left corner
        this->addChild(m_fields->m_heartSprite, 100);

        // Creating the BPM text layout
        m_fields->m_bpmLabel = CCLabelBMFont::create("80 BPM", "bigFont.fnt");
        m_fields->m_bpmLabel->setScale(0.4f);
        m_fields->m_bpmLabel->setAnchorPoint({0.0f, 0.5f});
        m_fields->m_bpmLabel->setPosition({75, 40});
        this->addChild(m_fields->m_bpmLabel, 100);

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        // Updating the heart rate every 0.7 seconds for smooth breathing effect
        m_fields->m_updateTimer += dt;
        if (m_fields->m_updateTimer >= 0.7f) {
            m_fields->m_updateTimer = 0.0f;

            if (!m_fields->m_bpmLabel || !m_fields->m_heartSprite) return;

            // Getting current progress percentage
            float currentPercent = this->getCurrentPercent();

            // Getting customizable values from the Geode settings menu
            auto endRange1 = Mod::get()->getSettingValue<int64_t>("range1-end");
            auto minBpm1 = Mod::get()->getSettingValue<int64_t>("range1-min-bpm");
            auto maxBpm1 = Mod::get()->getSettingValue<int64_t>("range1-max-bpm");
            
            auto minBpm2 = Mod::get()->getSettingValue<int64_t>("range2-min-bpm");
            auto maxBpm2 = Mod::get()->getSettingValue<int64_t>("range2-max-bpm");

            int targetMin = minBpm1;
            int targetMax = maxBpm1;

            // Checking if progress is past the first range trigger
            if (currentPercent > endRange1) {
                // Smoothly interpolating pulse value towards late-game max
                float progress = (currentPercent - endRange1) / (100.0f - endRange1);
                targetMin = static_cast<int>(minBpm1 + (minBpm2 - minBpm1) * progress);
                targetMax = static_cast<int>(maxBpm1 + (maxBpm2 - maxBpm1) * progress);
            }

            // Realtime randomizer to make the heart rate look alive
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(targetMin, targetMax);
            int currentBPM = distr(gen);

            // Updating string value on screen
            m_fields->m_bpmLabel->setString(fmt::format("{} BPM", currentBPM).c_str());

            // Pulsating heart animation effect
            m_fields->m_heartSprite->setScale(0.75f);
            m_fields->m_heartSprite->runAction(CCScaleTo::create(0.2f, 0.6f));
        }
    }
};
