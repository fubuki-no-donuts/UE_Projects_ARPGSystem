// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../ItemSystem/ItemInfo.h"
#include "Engine/DataTable.h"
#include "InventoryInfo.generated.h"

/**
 * ESlotType: 四种，两种库存形式（道具箱和装备箱），两种携带形式（道具栏和装备栏）
 */
UENUM(BlueprintType)
enum class ESlotType : uint8
{
	None UMETA(ToolTips = "None"),
	Common, // 道具箱和道具栏中的格子类型：可以盛放消耗品和素材
	Equipment // 装备箱中的格子类型：只要是装备即可
	// 对于装备栏，可以参考武器的细分类
};

/**
 * ESlotOwner：区分组织 Slot 的库存形式，方便分别需要从ItemInfo中取哪个上限数值
 */
UENUM(BlueprintType)
enum class ESlotOwner : uint8
{
	None,
	Box,
	Bar
};

/**
 * FInventoryInfo: 库存系统中道具的存储形式
 */
USTRUCT(BlueprintType)
struct FInventoryInfo
{
	GENERATED_BODY()

	// int32 SlotID; // 道具存放的格子 ID 作为 RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 ID; // 道具的唯一 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 Amount; // 物品数量，不同物品种类在不同的库存里堆叠上限不同（在 ItemSystem 中查阅）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EItemType Type; // 物品种类，用来判断放哪个库存里
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EEquipmentType EquipmentType; // 装备种类细分，判断装备格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 BoxCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 BarCapacity;

	void ResetInfo()
	{
		ID = 0;
		Amount = 0;
		Type = EItemType::None;
		EquipmentType = EEquipmentType::None;
		BoxCapacity = 0;
		BarCapacity = 0;
	}
	
};

/**
 * FInventorySlot: 库存系统中的道具格子，ItemBox、ItemBar、EquipBox 使用的格子
 */
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	FInventoryInfo InventoryInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	ESlotType SlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	EEquipmentType EquipSlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	int32 SlotID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	ESlotOwner SlotOwner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	int32 SlotCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	bool bIsDirty = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	bool bIsEmpty = true;

	virtual ~FInventorySlot() {}

	// 是否为空格子
	virtual bool IsSlotEmpty() const { return bIsEmpty; }

	/* 定义格子的基本操作 */

	// 检查物品是否和格子基础类型匹配（道具箱、道具栏和装备箱的逻辑），装备栏重写该判断
	virtual bool IsItemTypeMatchSlotType(const EItemType ItemType, const EEquipmentType EquipmentType) const
	{
		switch(SlotType)
		{
		case ESlotType::Common: // 道具箱或道具栏
			return (ItemType == EItemType::Consumable) || (ItemType == EItemType::Material);
		case ESlotType::Equipment: // 装备箱
			return ItemType == EItemType::Equipment;
		default:
			return false;
		}
	}

	// 格子接收物品
	virtual void AddItem(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		bAddSuccess = false;
		if(NewInventoryInfo.Amount <= 0 || !IsItemTypeMatchSlotType(NewInventoryInfo.Type, NewInventoryInfo.EquipmentType)) 
		{// 保险起见，这里判断一下添加的物品数量大于 0，然后判断是不是和格子类型匹配
			return ;
		}

		// 开始添加
		if(bIsEmpty) // 格子为空
		{
			// 先把其他信息填进去
			InventoryInfo = NewInventoryInfo;
			InventoryInfo.Amount = 0;
			AddItemToEmptySlot(NewInventoryInfo, bAddSuccess);
		}
		else // 格子不空
		{
			AddItemToOccupiedSlot(NewInventoryInfo, bAddSuccess);
		}
	}

	// 空格子添加物品
	virtual void AddItemToEmptySlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		// 根据是Box还是Bar选择对应的Capacity先赋值给SlotCapacity
		switch(SlotOwner)
		{
		case ESlotOwner::Box:
			SlotCapacity = NewInventoryInfo.BoxCapacity;
			break;
		case ESlotOwner::Bar:
			SlotCapacity = NewInventoryInfo.BarCapacity;
			break;
		default:
			bAddSuccess = false;
			return;
		}

		// 尝试添加到上限
		TryAddItemToCapacity(SlotCapacity, NewInventoryInfo, bAddSuccess);
	}

	// 给非空格子添加物品
	virtual void AddItemToOccupiedSlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		if(InventoryInfo.ID != NewInventoryInfo.ID || InventoryInfo.Amount >= SlotCapacity)
		{// 非空，要保证物品ID是同一个，并且已经有的物品没到上限
			return ;
		}

		// 格子不空的时候，SlotCapacity应当是已经设置过的，这里先不设置，后续有bug了再说
		// 尝试添加到上限
		const int32 LeftCapacity = SlotCapacity - InventoryInfo.Amount;
		TryAddItemToCapacity(LeftCapacity, NewInventoryInfo, bAddSuccess);
	}

	// 判断添加多少
	virtual void TryAddItemToCapacity(const int32 LeftCapacity, FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		if(NewInventoryInfo.Amount <= LeftCapacity) // 不超过上限
		{
			InventoryInfo.Amount += NewInventoryInfo.Amount; // 全添加
			NewInventoryInfo.ResetInfo(); // 都添加了就重置
		}
		else // 超出了上限
		{
			NewInventoryInfo.Amount -= LeftCapacity;
			InventoryInfo.Amount = SlotCapacity;
		}
		bIsEmpty = false;
		bAddSuccess = true; // 添加成功
	}


	// 格子移除物品
	virtual void RemoveItem(const int32 Amount, FInventoryInfo& OldInventoryInfo, bool& bRemoveSuccess)
	{
		bRemoveSuccess = false;

		if(!bIsEmpty || Amount<= 0 || Amount > InventoryInfo.Amount) // 格子为空或者移除数量为负或超出格子中盛放数目，移除失败，重置返回值
		{
			OldInventoryInfo.ResetInfo();
			return ;
		}

		OldInventoryInfo = InventoryInfo;
		if(Amount == InventoryInfo.Amount) // 全部移除
		{
			InventoryInfo.ResetInfo(); // 全移除了所以重置
			bIsEmpty = true; // 空了
			
		}
		else // 移除一部分
		{
			InventoryInfo.Amount -= Amount; // 移除一部分
			OldInventoryInfo.Amount = Amount; // 返回移除的部分
		}
		bRemoveSuccess = true;
	}
};


/**
 * FCommonSlot: ItemBox 和 ItemBar 使用的格子
 */
USTRUCT(BlueprintType)
struct FCommonSlot : public FInventorySlot
{
	GENERATED_BODY()

	FCommonSlot()
	{
		SlotType = ESlotType::Common;
		// Common 类格子要判断一下是Box还是Bar
		//SlotOwner = ESlotOwner::Box;
		//SlotOwner = ESlotOwner::Bar;
	}
};

/**
 * FEquipmentSlot: EquipmentBox 使用的格子
 */
USTRUCT(BlueprintType)
struct FEquipmentSlot : public FInventorySlot
{
	GENERATED_BODY()

	FEquipmentSlot()
	{
		SlotType = ESlotType::Equipment;
		SlotOwner = ESlotOwner::Box;
	}
};

/**
 * FEquipSlot: EquipBar 使用的格子
 */
USTRUCT(BlueprintType)
struct FEquipSlot : public FInventorySlot
{
	GENERATED_BODY()

	FEquipSlot()
	{
		SlotCapacity = 1;
		SlotType = ESlotType::Equipment;
		SlotOwner = ESlotOwner::Bar;
		EquipSlotType = EEquipmentType::None; // 在EquipBar中再具体设置
	}

	virtual bool IsItemTypeMatchSlotType(const EItemType ItemType, const EEquipmentType EquipmentType) const override
	{
		switch(EquipSlotType)
		{
			// 装备栏
			case EEquipmentType::Head:
				return EquipmentType == EEquipmentType::Head;
			case EEquipmentType::Chest:
				return EquipmentType == EEquipmentType::Chest;
			case EEquipmentType::Arms:
				return EquipmentType == EEquipmentType::Arms;
			case EEquipmentType::Waist:
				return EquipmentType == EEquipmentType::Waist;
			case EEquipmentType::Legs:
				return EquipmentType == EEquipmentType::Legs;
			case EEquipmentType::Ring:
				return EquipmentType == EEquipmentType::Ring;
			case EEquipmentType::Weapon:
				return EquipmentType == EEquipmentType::Weapon;
			default:
				return false;
		}
	}

	// 对装备栏，向已经有的Slot中添加，应当进行的其实是交换（slot更换为新的item，旧item发送到EquipmentBox中）
	// 所以逻辑基本上是 NewInventoryInfo 和 InventoryInfo 的信息进行交换
	virtual void AddItemToOccupiedSlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess) override
	{
		
	}
};


// TODO: 定义一个 InventoryTable 的 struct
USTRUCT(BlueprintType)
struct FInventorySlotDT : public FTableRowBase
{
	GENERATED_BODY()

	// int32 SlotID; 存放的格子 ID 作为 RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 ID; // 道具的唯一 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 Amount; // 物品数量，不同物品种类在不同的库存里堆叠上限不同（在 ItemSystem 中查阅）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EItemType Type; // 物品种类，用来判断放哪个库存里
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EEquipmentType EquipmentType; // 装备种类细分，判断装备格子
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 BoxCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 BarCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	ESlotType SlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	EEquipmentType EquipSlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	int32 SlotCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	int32 SlotOwner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	bool bIsDirty;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Slot")
	bool bIsEmpty;
};






