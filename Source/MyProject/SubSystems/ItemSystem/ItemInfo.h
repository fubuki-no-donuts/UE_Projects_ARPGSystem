// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemInfo.generated.h"

/* EItemType：道具系统定义的道具最基础的分类：消耗品、装备、素材 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(ToolTips = "ItemType::None"),
	Consumable UMETA(ToolTips = "ItemType::Consumable"),
	Material  UMETA(ToolTips = "ItemType::Material"),
	Equipment UMETA(ToolTips = "ItemType::Equipment")
};

/* EEquipmentType: 道具系统中具体定义装备的细分 */
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	// 这里和 InventorySystem 中的 EquipBar 对应
	Head, // 头盔
	Chest, // 铠甲
	Arms, // 腕甲
	Waist, // 腰甲
	Legs, // 护腿
	Ring, // 护石
	Weapon // 武器
};

/* EItemQuality: 道具系统定义道具的质量分级 */
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
 * FItemInfoTable：全道具（除装备外）信息表（后续根据其他系统需要，再逐步添加新的属性）
 */
USTRUCT(BlueprintType)
struct FItemInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	// int32 ID; // Table 的 RowName 为道具的唯一 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	EItemType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	EEquipmentType EquipmentType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Item Table")
	FName Name; // 道具名称
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


