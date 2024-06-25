// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemInfo.generated.h"

/* EItemType������ϵͳ����ĵ���������ķ��ࣺ����Ʒ��װ�����ز� */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(ToolTips = "ItemType::None"),
	Consumable UMETA(ToolTips = "ItemType::Consumable"),
	Material  UMETA(ToolTips = "ItemType::Material"),
	Equipment UMETA(ToolTips = "ItemType::Equipment")
};

/* EEquipmentType: ����ϵͳ�о��嶨��װ����ϸ�� */
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	// ����� InventorySystem �е� EquipBar ��Ӧ
	Head, // ͷ��
	Chest, // ����
	Arms, // ���
	Waist, // ����
	Legs, // ����
	Ring, // ��ʯ
	Weapon // ����
};

/* EItemQuality: ����ϵͳ������ߵ������ּ� */
UENUM(BlueprintType)
enum class EItemQuality : uint8
{
	None UMETA(ToolTips = "ItemQuality::None"),
	Rare1 UMETA(ToolTips = "ItemQuality::Rare1"),
	Rare2 UMETA(ToolTips = "ItemQuality::Rare2"),
	Rare3 UMETA(ToolTips = "ItemQuality::Rare3"),
	Rare4 UMETA(ToolTips = "ItemQuality::Rare4"),
	Rare5 UMETA(ToolTips = "ItemQuality::Rare5")
};

/**
 * FItemInfoTable��ȫ���ߣ���װ���⣩��Ϣ��������������ϵͳ��Ҫ����������µ����ԣ�
 */
USTRUCT(BlueprintType)
struct FItemInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	// int32 ID; // Table �� RowName Ϊ���ߵ�Ψһ ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	EItemType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	EEquipmentType EquipmentType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	FName Name; // ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	FName Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	class UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	EItemQuality Quality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	int32 BoxCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	int32 BarCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	int32 CoinCost;
};


namespace ItemTools
{
	static int32 FNameToInt32(const FName& Name)
	{
		const FString NameString = Name.ToString();
		return FCString::Atoi(*NameString);
	}
	static FName Int32ToFName(const int32 Number)
	{
		const FString NumberString = FString::FromInt(Number);
		return FName(*NumberString);
	}
};


