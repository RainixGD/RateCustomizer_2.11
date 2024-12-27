#include "./includes.h"

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)
bool operator==(const cocos2d::CCPoint& a, const cocos2d::CCPoint& b) { return a.x == b.x && a.y == b.y; }
bool operator==(const cocos2d::CCRect& a, const cocos2d::CCRect& b) { return a.origin == b.origin && a.size == b.size; }

class BadgeCustomizerManager {
	enum DataLoadingResult {
		OK,
		FileNotFound,
		ParsingError
	};
	std::unordered_map<unsigned int, std::string> badges;
	DataLoadingResult loadingStatus;
	static BadgeCustomizerManager* instance;

	void init() {
		loadingStatus = loadData();
	}

	bool can_convert_to_int(const std::string& str) {
		try {
			std::stoi(str);
			return true;
		}
		catch (const std::invalid_argument& e) {
			return false;
		}
		catch (const std::out_of_range& e) {
			return false;
		}
	}

	DataLoadingResult loadData() {
		std::ifstream file("Resources/rateCustomizer.json");
		if (!file) return FileNotFound;
		std::ostringstream buffer;
		buffer << file.rdbuf();
		std::string fileContent = buffer.str();

		file.close();
		try {
			auto root = nlohmann::json::parse(fileContent);

			if (!root.is_object()) return ParsingError;

			for (const auto& [key, value] : root.items()) {
				if (!can_convert_to_int(key)) return ParsingError;

				int rateId = std::stoi(key);
				if (rateId < 1 || !value.is_string()) return ParsingError;

				badges[rateId] = value.get<std::string>();
			}
		}
		catch (const std::exception& e) {
			return ParsingError;
		}
		return OK;
	}

	void createErrorLabel(CCLayer* layer) {
		std::string errorText;
		switch (loadingStatus) {
		case BadgeCustomizerManager::FileNotFound:
			errorText = "Can't find 'rateCustomizer.json' in ./Resources";
			break;
		case BadgeCustomizerManager::ParsingError:
			errorText = "Can't parse 'rateCustomizer.json'";
			break;
		}

		auto size = CCDirector::sharedDirector()->getWinSize();

		auto errorLabel = CCLabelBMFont::create(errorText.c_str(), "bigFont.fnt");
		errorLabel->setColor({ 255, 0, 0 });
		errorLabel->setScale(0.6);
		errorLabel->setPosition({ size.width / 2, size.height - 10 });
		layer->addChild(errorLabel);
	}

	BadgeCustomizerManager() {};
public:

	void onMenuLayer(CCLayer* layer) {
		if (loadingStatus != OK) {
			createErrorLabel(layer);
			return;
		}
	}

	void onLevelInfoLayer_setupLevelInfo(LevelInfoLayer* layer) {
		if (loadingStatus != OK) return;

		int rateValue = layer->m_pLevel->featured;
		CCPoint rateCoinPos;

		CCObject* obj;
		CCARRAY_FOREACH(layer->getChildren(), obj) {
			auto NodeObj = reinterpret_cast<CCNode*>(obj);
			if (auto sprite_node = dynamic_cast<CCSprite*>(obj); sprite_node) {
				auto* texture = sprite_node->getTexture();
				CCDictElement* el;
				auto* frame_cache = CCSpriteFrameCache::sharedSpriteFrameCache();
				auto* cached_frames = public_cast(frame_cache, m_pSpriteFrames);
				const auto rect = sprite_node->getTextureRect();
				CCDICT_FOREACH(cached_frames, el) {
					auto* frame = static_cast<CCSpriteFrame*>(el->getObject());
					if (frame->getTexture() == texture && frame->getRect() == rect) {
						if (std::string(el->getStrKey()) == std::string("GJ_featuredCoin_001.png") || std::string(el->getStrKey()) == std::string("GJ_epicCoin_001.png")) {
							rateCoinPos = NodeObj->getPosition();
							NodeObj->setScale(0);
						}
						break;
					}
				}
			}
		}
		if (rateValue > 0) {
			if (badges.find(rateValue) == badges.end()) return;
			auto newRateIcon = CCSprite::create(badges[rateValue].c_str());
			if (newRateIcon == nullptr) {
				newRateIcon = CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png");
				newRateIcon->setZOrder(5);
			}
			newRateIcon->setPosition({ rateCoinPos.x, rateCoinPos.y + 8 });
			layer->addChild(newRateIcon);
		}
	}

	void onLevelCell_loadCustomLevelCell(LevelCell* cell) {
		if (loadingStatus != OK) return;

		int rateValue = cell->m_pLevel->featured;
		CCPoint rateCoinPos;

		auto newSelfObj = cell->getChildren()->objectAtIndex(1);
		auto newSelf = dynamic_cast<CCNode*>(newSelfObj);

		CCObject* obj;
		CCARRAY_FOREACH(newSelf->getChildren(), obj) {
			auto NodeObj = reinterpret_cast<CCNode*>(obj);
			if (auto sprite_node = dynamic_cast<CCSprite*>(obj); sprite_node) {
				auto* texture = sprite_node->getTexture();
				CCDictElement* el;
				auto* frame_cache = CCSpriteFrameCache::sharedSpriteFrameCache();
				auto* cached_frames = public_cast(frame_cache, m_pSpriteFrames);
				const auto rect = sprite_node->getTextureRect();
				CCDICT_FOREACH(cached_frames, el) {
					auto* frame = static_cast<CCSpriteFrame*>(el->getObject());
					if (frame->getTexture() == texture && frame->getRect() == rect) {
						if (std::string(el->getStrKey()) == std::string("GJ_featuredCoin_001.png") || std::string(el->getStrKey()) == std::string("GJ_epicCoin_001.png")) {
							rateCoinPos = NodeObj->getPosition();
							NodeObj->removeFromParent();
						}
						break;
					}
				}
			}
		}
		if (rateValue > 0) {
			if (badges.find(rateValue) == badges.end()) return;
			auto newRateIcon = CCSprite::create(badges[rateValue].c_str());
			if (newRateIcon == nullptr) {
				newRateIcon = CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png");
				newRateIcon->setZOrder(5);
			}
			newRateIcon->setPosition({ rateCoinPos.x, rateCoinPos.y + 8 });
			newSelf->addChild(newRateIcon);
		}
	}

	static BadgeCustomizerManager* getInstance() {
		if (!instance) {
			instance = new BadgeCustomizerManager();
			instance->init();
		}
		return instance;
	}
};
BadgeCustomizerManager* BadgeCustomizerManager::instance = nullptr;

bool(__thiscall* MenuLayer_init)(MenuLayer* self);
bool __fastcall MenuLayer_init_H(MenuLayer* self, void*) {
	if (!MenuLayer_init(self)) return false;
	BadgeCustomizerManager::getInstance()->onMenuLayer(self);
	return true;
}

void(__thiscall* LevelInfoLayer_setupLevelInfo)(LevelInfoLayer* self);
void __fastcall LevelInfoLayer_setupLevelInfo_H(LevelInfoLayer* self) {
	LevelInfoLayer_setupLevelInfo(self);
	BadgeCustomizerManager::getInstance()->onLevelInfoLayer_setupLevelInfo(self);
}

void(__thiscall* LevelCell_loadCustomLevelCell)(LevelCell* cell);
void __fastcall LevelCell_loadCustomLevelCell_H(LevelCell* cell) {
	LevelCell_loadCustomLevelCell(cell);
	BadgeCustomizerManager::getInstance()->onLevelCell_loadCustomLevelCell(cell);
}

void inject() {
#if _WIN32
	auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

	MH_CreateHook(
		reinterpret_cast<void*>(base + 0x1907b0),
		reinterpret_cast<void*>(&MenuLayer_init_H),
		reinterpret_cast<void**>(&MenuLayer_init)
	);

	MH_CreateHook(
		reinterpret_cast<void*>(base + 0x178680),
		reinterpret_cast<void*>(&LevelInfoLayer_setupLevelInfo_H),
		reinterpret_cast<void**>(&LevelInfoLayer_setupLevelInfo)
	);

	MH_CreateHook(
		reinterpret_cast<void*>(base + 0x5a020),
		reinterpret_cast<void*>(&LevelCell_loadCustomLevelCell_H),
		reinterpret_cast<void**>(&LevelCell_loadCustomLevelCell)
	);

	MH_EnableHook(MH_ALL_HOOKS);
#endif
}

#if _WIN32
WIN32CAC_ENTRY(inject)
#endif