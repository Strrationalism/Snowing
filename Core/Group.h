#pragma once
#include <algorithm>
#include <vector>
#include <queue>
#include <memory>
#include "Object.h"
#include "MemPool.h"

namespace Snowing::Scene
{
	template <typename TBaseObject = Object>
	class[[nodiscard]] Group : public Object
	{
	private:
		struct ObjectWrapper
		{
			bool living = true;
			std::unique_ptr<TBaseObject> object;

			ObjectWrapper(std::unique_ptr<TBaseObject>&& obj) :
				object{ std::move(obj) }
			{};
		};

		std::vector<ObjectWrapper> objs_;
		std::queue<std::unique_ptr<TBaseObject>> newObjs_;
	public:

		void PrepareNewObjects()
		{
			while (!newObjs_.empty())
			{
				objs_.emplace_back(std::move(newObjs_.front()));
				newObjs_.pop();
			}
		}

		bool Update() override
		{
			PrepareNewObjects();

			for (auto& p : objs_)
				p.living = p.object->Update();

			const auto eraseBegin = std::remove_if(
				objs_.begin(),
				objs_.end(),
				[](const auto & p) { return !p.living; });

			objs_.erase(
				eraseBegin,
				objs_.end());

			return !objs_.empty();
		}

		template <typename TObj = TBaseObject,typename ... TArgs>
		TObj* Emplace(TArgs&&... args)
		{
			auto p = std::make_unique<TObj>(std::forward<TArgs>(args)...);
			auto ptr = p.get();
			newObjs_.emplace(std::move(p));
			return ptr;
		}

		template <typename TObj>
		TObj* PushBack(std::unique_ptr<TObj>&& obj)
		{
			auto p = obj.get();
			newObjs_.emplace(std::move(obj));
			return p;
		}

		template <typename TFunc>
		void Iter(TFunc& f)
		{
			PrepareNewObjects();
			for (auto& p : objs_)
				f(*p.object);
		}

		template <typename TFunc>
		void Sort(TFunc& f)
		{
			PrepareNewObjects();
			std::sort(objs_.begin(), objs_.end(), [&f](auto& p,auto& q) {
				return f(*p.object,*q.object);
			});
		}

		template <typename TObjectType, typename TFunc>
		void IterType(TFunc& f)
		{
			PrepareNewObjects();
			Iter([this,&f](TBaseObject& obj) {
				try
				{
					f(dynamic_cast<TObjectType&>(obj));
				}
				catch(std::bad_cast)
				{ }
			});
		}
		
		template <typename TObjectType>
		TObjectType * FindFirstIgnoreNewObjects() const
		{
			for(auto& p : objs_)
			{
				try
				{
					auto ptr = dynamic_cast<TObjectType*>(p.object.get());
					if (ptr) return ptr;
				}
				catch(std::bad_cast)
				{ }
			}
			return nullptr;
		}

		template <typename TObjectType>
		TObjectType* FindFirst()
		{
			PrepareNewObjects();
			return FindFirstIgnoreNewObjects<TObjectType>();
		}

		void Clear()
		{
			objs_.clear();
			while (!newObjs_.empty())
				newObjs_.pop();
		}

		size_t Count() const
		{
			return objs_.size() + newObjs_.size();
		}

		TBaseObject* operator [] (size_t i)
		{
			PrepareNewObjects();
			return objs_[i].object.get();
		}

		bool ExistIgnoreNewObjects(const TBaseObject* pObject) const
		{
			return std::any_of(objs_.begin(), objs_.end(), [pObject](const auto& pObjectInList)
			{
				return pObject == pObjectInList.object.get();
			});
		}

		bool Exist(const TBaseObject* pObject)
		{
			PrepareNewObjects();
			return ExistIgnoreNewObjects(pObject);
		}

		template <typename TCond>
		TBaseObject* FindFirstIgnoreNewObjects(TCond& cond)
		{
			for (auto& p : objs_)
			{
				if (cond(*p.object))
					return p.object.get();
			}

			return nullptr;
		}

		template <typename TCond>
		TBaseObject* FindFirst(TCond& cond)
		{
			PrepareNewObjects();
			return FindFirstIgnoreNewObjects(cond);
		}
	};

	template <typename TBaseObject = Object>
	class FinalGroup : public Group<TBaseObject>, public MemPool<FinalGroup<TBaseObject>>
	{
	};
}
