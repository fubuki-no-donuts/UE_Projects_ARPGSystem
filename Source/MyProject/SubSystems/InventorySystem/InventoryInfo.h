// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../ItemSystem/ItemInfo.h"
#include "Engine/DataTable.h"
#include "InventoryInfo.generated.h"

/**
 * ESlotType: ���֣����ֿ����ʽ���������װ���䣩������Я����ʽ����������װ������
 */
UENUM(BlueprintType)
enum class ESlotType : uint8
{
	None UMETA(ToolTips = "None"),
	Common, // ������͵������еĸ������ͣ�����ʢ������Ʒ���ز�
	Equipment // װ�����еĸ������ͣ�ֻҪ��װ������
	// ����װ���������Բο�������ϸ����
};

/**
 * ESlotOwner��������֯ Slot �Ŀ����ʽ������ֱ���Ҫ��ItemInfo��ȡ�ĸ�������ֵ
 */
UENUM(BlueprintType)
enum class ESlotOwner : uint8
{
	None,
	Box,
	Bar
};

/**
 * FInventoryInfo: ���ϵͳ�е��ߵĴ洢��ʽ
 */
USTRUCT(BlueprintType)
struct FInventoryInfo
{
	GENERATED_BODY()

	// int32 SlotID; // ���ߴ�ŵĸ��� ID ��Ϊ RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 ID; // ���ߵ�Ψһ ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 Amount; // ��Ʒ��������ͬ��Ʒ�����ڲ�ͬ�Ŀ����ѵ����޲�ͬ���� ItemSystem �в��ģ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EItemType Type; // ��Ʒ���࣬�����жϷ��ĸ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EEquipmentType EquipmentType; // װ������ϸ�֣��ж�װ������
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
 * FInventorySlot: ���ϵͳ�еĵ��߸��ӣ�ItemBox��ItemBar��EquipBox ʹ�õĸ���
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

	// �Ƿ�Ϊ�ո���
	virtual bool IsSlotEmpty() const { return bIsEmpty; }

	/* ������ӵĻ������� */

	// �����Ʒ�Ƿ�͸��ӻ�������ƥ�䣨�����䡢��������װ������߼�����װ������д���ж�
	virtual bool IsItemTypeMatchSlotType(const EItemType ItemType, const EEquipmentType EquipmentType) const
	{
		switch(SlotType)
		{
		case ESlotType::Common: // ������������
			return (ItemType == EItemType::Consumable) || (ItemType == EItemType::Material);
		case ESlotType::Equipment: // װ����
			return ItemType == EItemType::Equipment;
		default:
			return false;
		}
	}

	// ���ӽ�����Ʒ
	virtual void AddItem(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		bAddSuccess = false;
		if(NewInventoryInfo.Amount <= 0 || !IsItemTypeMatchSlotType(NewInventoryInfo.Type, NewInventoryInfo.EquipmentType)) 
		{// ��������������ж�һ����ӵ���Ʒ�������� 0��Ȼ���ж��ǲ��Ǻ͸�������ƥ��
			return ;
		}

		// ��ʼ���
		if(bIsEmpty) // ����Ϊ��
		{
			// �Ȱ�������Ϣ���ȥ
			InventoryInfo = NewInventoryInfo;
			InventoryInfo.Amount = 0;
			AddItemToEmptySlot(NewInventoryInfo, bAddSuccess);
		}
		else // ���Ӳ���
		{
			AddItemToOccupiedSlot(NewInventoryInfo, bAddSuccess);
		}
	}

	// �ո��������Ʒ
	virtual void AddItemToEmptySlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		// ������Box����Barѡ���Ӧ��Capacity�ȸ�ֵ��SlotCapacity
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

		// ������ӵ�����
		TryAddItemToCapacity(SlotCapacity, NewInventoryInfo, bAddSuccess);
	}

	// ���ǿո��������Ʒ
	virtual void AddItemToOccupiedSlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		if(InventoryInfo.ID != NewInventoryInfo.ID || InventoryInfo.Amount >= SlotCapacity)
		{// �ǿգ�Ҫ��֤��ƷID��ͬһ���������Ѿ��е���Ʒû������
			return ;
		}

		// ���Ӳ��յ�ʱ��SlotCapacityӦ�����Ѿ����ù��ģ������Ȳ����ã�������bug����˵
		// ������ӵ�����
		const int32 LeftCapacity = SlotCapacity - InventoryInfo.Amount;
		TryAddItemToCapacity(LeftCapacity, NewInventoryInfo, bAddSuccess);
	}

	// �ж���Ӷ���
	virtual void TryAddItemToCapacity(const int32 LeftCapacity, FInventoryInfo& NewInventoryInfo, bool& bAddSuccess)
	{
		if(NewInventoryInfo.Amount <= LeftCapacity) // ����������
		{
			InventoryInfo.Amount += NewInventoryInfo.Amount; // ȫ���
			NewInventoryInfo.ResetInfo(); // ������˾�����
		}
		else // ����������
		{
			NewInventoryInfo.Amount -= LeftCapacity;
			InventoryInfo.Amount = SlotCapacity;
		}
		bIsEmpty = false;
		bAddSuccess = true; // ��ӳɹ�
	}


	// �����Ƴ���Ʒ
	virtual void RemoveItem(const int32 Amount, FInventoryInfo& OldInventoryInfo, bool& bRemoveSuccess)
	{
		bRemoveSuccess = false;

		if(!bIsEmpty || Amount<= 0 || Amount > InventoryInfo.Amount) // ����Ϊ�ջ����Ƴ�����Ϊ���򳬳�������ʢ����Ŀ���Ƴ�ʧ�ܣ����÷���ֵ
		{
			OldInventoryInfo.ResetInfo();
			return ;
		}

		OldInventoryInfo = InventoryInfo;
		if(Amount == InventoryInfo.Amount) // ȫ���Ƴ�
		{
			InventoryInfo.ResetInfo(); // ȫ�Ƴ�����������
			bIsEmpty = true; // ����
			
		}
		else // �Ƴ�һ����
		{
			InventoryInfo.Amount -= Amount; // �Ƴ�һ����
			OldInventoryInfo.Amount = Amount; // �����Ƴ��Ĳ���
		}
		bRemoveSuccess = true;
	}
};


/**
 * FCommonSlot: ItemBox �� ItemBar ʹ�õĸ���
 */
USTRUCT(BlueprintType)
struct FCommonSlot : public FInventorySlot
{
	GENERATED_BODY()

	FCommonSlot()
	{
		SlotType = ESlotType::Common;
		// Common �����Ҫ�ж�һ����Box����Bar
		//SlotOwner = ESlotOwner::Box;
		//SlotOwner = ESlotOwner::Bar;
	}
};

/**
 * FEquipmentSlot: EquipmentBox ʹ�õĸ���
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
 * FEquipSlot: EquipBar ʹ�õĸ���
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
		EquipSlotType = EEquipmentType::None; // ��EquipBar���پ�������
	}

	virtual bool IsItemTypeMatchSlotType(const EItemType ItemType, const EEquipmentType EquipmentType) const override
	{
		switch(EquipSlotType)
		{
			// װ����
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

	// ��װ���������Ѿ��е�Slot����ӣ�Ӧ�����е���ʵ�ǽ�����slot����Ϊ�µ�item����item���͵�EquipmentBox�У�
	// �����߼��������� NewInventoryInfo �� InventoryInfo ����Ϣ���н���
	virtual void AddItemToOccupiedSlot(FInventoryInfo& NewInventoryInfo, bool& bAddSuccess) override
	{
		
	}
};


// TODO: ����һ�� InventoryTable �� struct
USTRUCT(BlueprintType)
struct FInventorySlotDT : public FTableRowBase
{
	GENERATED_BODY()

	// int32 SlotID; ��ŵĸ��� ID ��Ϊ RowName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 ID; // ���ߵ�Ψһ ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	int32 Amount; // ��Ʒ��������ͬ��Ʒ�����ڲ�ͬ�Ŀ����ѵ����޲�ͬ���� ItemSystem �в��ģ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EItemType Type; // ��Ʒ���࣬�����жϷ��ĸ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Inventory Info")
	EEquipmentType EquipmentType; // װ������ϸ�֣��ж�װ������
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






