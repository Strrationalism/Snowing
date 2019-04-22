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
		std::vector<std::unique_ptr<TBaseObject>> objs_;
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
			objs_.erase(
				std::remove_if(
					objs_.begin(),
					objs_.end(),
					[](auto& p) { return !p->Update(); }),
				objs_.end());

			PrepareNewObjects();

			return !objs_.empty();
		}

		template <typename TObj = TBaseObject,typename ... TArgs>
		TObj* Emplace(TArgs&&... args)
		{
			auto p = new TObj{ std::forward<TArgs>(args)... };
			newObjs_.emplace(std::unique_ptr<TObj>(p));
			return p;
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
				f(*p);
		}

		template <typename TFunc>
		void Sort(TFunc& f)
		{
			PrepareNewObjects();
			std::sort(objs_.begin(), objs_.end(), [&f](auto& p) {
				return f(*p);
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
		TObjectType * FindFirst()
		{
			PrepareNewObjects();
			for(auto& p : objs_)
			{
				try
				{
					auto ptr = dynamic_cast<TObjectType*>(p.get());
					if (ptr) return ptr;
				}
				catch(std::bad_cast)
				{ }
			}
			return nullptr;
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
			return objs_[i].get();
		}

		bool ExistIgnoreNewObjects(TBaseObject* pObject) const
		{
			return std::any_of(objs_.begin(), objs_.end(), [pObject](const auto& pObjectInList)
			{
				return pObject == pObjectInList.get();
			});
		}

		bool Exist(TBaseObject* pObject)
		{
			PrepareNewObjects();
			return ExistIgnoreNewObjects(pObject);
		}
	};

	template <typename TBaseObject = Object>
	class FinalGroup : public Group<TBaseObject>, public MemPool<FinalGroup<TBaseObject>>
	{
	};
}
