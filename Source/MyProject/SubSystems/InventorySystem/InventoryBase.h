// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryInfo.h"
#include "InventoryBase.generated.h"

/**
 * EInventoryType: ���֣����ֿ����ʽ���������װ���䣩������Я����ʽ����������װ������
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
 * UInventoryBase: ������
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
	void InitializeSlots(TArray<T>& InventorySlots, const ESlotType SlotType, const ESlotOwner SlotOwner) // ���ݿ�����ͳ�ʼ������
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

	void LoadToSlots() // �Ӵ洢���ж�ȡ���ݵ�����
	{
		
	}
	void SaveFromSlots() // ���������ݱ��浽�洢����
	{
		
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	EInventoryType InventoryType; // ������ͣ������䡢��������װ���䡢װ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	int32 Capacity; // ����ܷŶ��ٸ����ӣ�box Ϊ96��item bar ��24(3 * 8)��equip bar ��Ҫ�ָ��Ӿ�������ͣ�ͷ���ء��ŵȣ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	UDataTable* DataTable; // ÿ�������ʽ���ж�Ӧ��һ�ݴ洢��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Base")
	FString DataTablePath; // �洢��·��
};

/**
 * UInventoryBox: ��������
 */
UCLASS()
class MYPROJECT_API UInventoryBox : public UInventoryBase
{
	GENERATED_BODY()

public:
};

/**
 * UInventoryBar: ���������
 */
UCLASS()
class MYPROJECT_API UInventoryBar : public UInventoryBase
{
	GENERATED_BODY()

public:
};


/* ��������ִ洢��ʽ */

/**
 * UItemBox��������
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
		Capacity = 96; // �Ѿ��� UInventoryBox ������
		InitializeSlots<FCommonSlot>(ItemSlots, ESlotType::Common, ESlotOwner::Box);
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Item Box", meta=(AllowPrivateAccess="true"))
	TArray<FCommonSlot> ItemSlots;
};

/**
 * UEquipmentBox��������
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
		Capacity = 96; // �Ѿ��� UInventoryBox ������
		InitializeSlots<FEquipmentSlot>(EquipmentSlots, ESlotType::Equipment, ESlotOwner::Box);
	}
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Equipment Box", meta=(AllowPrivateAccess="true"))
	TArray<FEquipmentSlot> EquipmentSlots;
};

/**
 * UItemBar��������
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
		Capacity = 24; // ��ɫ��24������Я������
		InitializeSlots<FCommonSlot>(ItemSlots, ESlotType::Common, ESlotOwner::Bar);
	}
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Item Bar", meta=(AllowPrivateAccess="true"))
	TArray<FCommonSlot> ItemSlots;
};

/**
 * UEquipBar��������
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
		Capacity = 7; // ��ɫ�� 7 ��װ����λ
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


