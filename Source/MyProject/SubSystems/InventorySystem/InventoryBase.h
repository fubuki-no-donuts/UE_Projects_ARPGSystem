// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryInfo.h"
#include "InventoryBase.generated.h"

/**
 * EInventoryType: 四种，两种库存形式（道具箱和装备箱），两种携带形式（道具栏和装备栏）
 */
UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	None UMETA(ToolTips = "None"),
	ItemBox UMETA(ToolTips = "Player's item inventory"),
	EquipBox UMETA(ToolTips = "Player's equipment inventory"),
	ItemBar UMETA(ToolTips = "Props carried by player"),
	EquipBar UMETA(ToolTips = "Equipment carried by player")
};

/**
 * UInventoryBase: 库存基类
 */
UCLASS(Abstract)
class MYPROJECT_API UInventoryBase : public UObject
{
	GENERATED_BODY()

public:
	UInventoryBase()
	{}

protected:
	template<typename T>
	void InitializeSlots(TArray<T>& InventorySlots, const ESlotType SlotType, const ESlotOwner SlotOwner) // 根据库存类型初始化格子
	{
		T InventorySlot;
		InventorySlot.SlotType = SlotType;
		InventorySlot.SlotOwner = SlotOwner;

		InventorySlots.Init(InventorySlot, Capacity);
		InventorySlots.Shrink();

		for(SIZE_T Index = 0; Index < InventorySlots.Num(); Index ++)
		{
			InventorySlots[Index].SlotID = Index;
		}
	}

	void LoadTable()
	{
		//if(!FPackageName::DoesPackageExist(DataTablePath))
		//{
		//	return ;
		//}

		//DataTable = LoadObject<UDataTable>(this, *DataTablePath);
	}

	void LoadToSlots() // 从存储表中读取数据到格子
	{
		
	}
	void SaveFromSlots() // 将格子数据保存到存储表中
	{
		
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	EInventoryType InventoryType; // 库存类型：道具箱、道具栏、装备箱、装备栏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	int32 Capacity; // 库存能放多少个格子，box 为96，item bar 是24(3 * 8)，equip bar 需要分格子具体的类型（头、胸、脚等）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	UDataTable* DataTable; // 每个库存形式都有对应的一份存储表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	FString DataTablePath; // 存储表路径
};

/**
 * UInventoryBox: 库存箱基类
 */
UCLASS()
class MYPROJECT_API UInventoryBox : public UInventoryBase
{
	GENERATED_BODY()

public:
};

/**
 * UInventoryBar: 库存栏基类
 */
UCLASS()
class MYPROJECT_API UInventoryBar : public UInventoryBase
{
	GENERATED_BODY()

public:
};


/* 具体的四种存储形式 */

/**
 * UItemBox：道具箱
 */
UCLASS()
class MYPROJECT_API UItemBox final : public UInventoryBox
{
	GENERATED_BODY()

public:
	UItemBox()
	{
		LoadTable();
		InventoryType = EInventoryType::ItemBox;
		Capacity = 96; // 已经在 UInventoryBox 中设置
		InitializeSlots<FCommonSlot>(ItemSlots, ESlotType::Common, ESlotOwner::Box);
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Item Box", meta=(AllowPrivateAccess="true"))
	TArray<FCommonSlot> ItemSlots;
};

/**
 * UEquipmentBox：武器箱
 */
UCLASS()
class MYPROJECT_API UEquipmentBox final : public UInventoryBox
{
	GENERATED_BODY()

public:
	UEquipmentBox()
	{
		LoadTable();
		InventoryType = EInventoryType::EquipBox;
		Capacity = 96; // 已经在 UInventoryBox 中设置
		InitializeSlots<FEquipmentSlot>(EquipmentSlots, ESlotType::Equipment, ESlotOwner::Box);
	}
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Equipment Box", meta=(AllowPrivateAccess="true"))
	TArray<FEquipmentSlot> EquipmentSlots;
};

/**
 * UItemBar：道具栏
 */
UCLASS()
class MYPROJECT_API UItemBar final : public UInventoryBar
{
	GENERATED_BODY()

public:
	UItemBar()
	{
		LoadTable();
		InventoryType = EInventoryType::ItemBar;
		Capacity = 24; // 角色有24个道具携带格子
		InitializeSlots<FCommonSlot>(ItemSlots, ESlotType::Common, ESlotOwner::Bar);
	}
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Item Bar", meta=(AllowPrivateAccess="true"))
	TArray<FCommonSlot> ItemSlots;
};

/**
 * UEquipBar：武器栏
 */
UCLASS()
class MYPROJECT_API UEquipBar final : public UInventoryBar
{
	GENERATED_BODY()

public:
	UEquipBar()
	{
		LoadTable();
		InventoryType = EInventoryType::EquipBar;
		Capacity = 7; // 角色的 7 个装备槽位
		InitializeSlots<FEquipSlot>(EquipSlots, ESlotType::Equipment, ESlotOwner::Bar);

		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Head)-1].EquipSlotType = EEquipmentType::Head;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Chest)-1].EquipSlotType = EEquipmentType::Chest;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Arms)-1].EquipSlotType = EEquipmentType::Arms;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Waist)-1].EquipSlotType = EEquipmentType::Waist;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Legs)-1].EquipSlotType = EEquipmentType::Legs;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Ring)-1].EquipSlotType = EEquipmentType::Ring;
		EquipSlots[static_cast<SIZE_T>(EEquipmentType::Weapon)-1].EquipSlotType = EEquipmentType::Weapon;
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Equip Bar", meta=(AllowPrivateAccess="true"))
	TArray<FEquipSlot> EquipSlots;
};


