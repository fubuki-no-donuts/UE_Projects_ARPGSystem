// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemInfo.h"
#include "ItemSystem.generated.h"

/* 前向声明 */
class UDataTable;

/**
 * 道具系统
 *		全局道具信息表
 */
UCLASS()
class MYPROJECT_API UItemSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category= "Item System")
	void CheckItemByID(const int32 ItemID, bool& bIsExist) const;

	UFUNCTION(BlueprintCallable, Category= "Item System")
	FORCEINLINE UDataTable* GetItemsTable() const { return AllItemsTable; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Item System")
	UDataTable* AllItemsTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Item System")
	UDataTable* AllEquipmentsTable;


private:
	UPROPERTY(VisibleAnywhere, Category= "Item System")
	FString AllItemsTablePath;
	UPROPERTY(VisibleAnywhere, Category= "Item System")
	FString AllEquipmentsTablePath;
};




