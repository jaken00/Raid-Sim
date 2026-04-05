using System.Text.Json.Serialization;

namespace RaidSimTools.Models;

public class BossFile
{
    [JsonPropertyName("bosses")]
    public List<Boss> Bosses { get; set; } = [];
}

public class Boss
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("raid")]
    public string Raid { get; set; } = "";

    [JsonPropertyName("max_hp")]
    public int MaxHp { get; set; }

    [JsonPropertyName("phase_count")]
    public int PhaseCount { get; set; }

    [JsonPropertyName("tuning_ilvl")]
    public int TuningIlvl { get; set; }

    [JsonPropertyName("hps_threshold")]
    public int HpsThreshold { get; set; }

    [JsonPropertyName("dps_threshold")]
    public int DpsThreshold { get; set; }

    [JsonPropertyName("interrupt_coverage_needed")]
    public int InterruptCoverageNeeded { get; set; }

    [JsonPropertyName("tank_minimum")]
    public int TankMinimum { get; set; }

    [JsonPropertyName("dispel_coverage_needed")]
    public int DispelCoverageNeeded { get; set; }

    [JsonPropertyName("rewards_physical_buffs")]
    public bool RewardsPhysicalBuffs { get; set; }

    [JsonPropertyName("punishes_melee_heavy")]
    public bool PunishesMeleeHeavy { get; set; }

    [JsonPropertyName("damage_type")]
    public string DamageType { get; set; } = "";

    [JsonPropertyName("resistance_profile")]
    public ResistanceProfile Resistances { get; set; } = new();

    [JsonPropertyName("phases")]
    public List<BossPhase> Phases { get; set; } = [];
}

public class ResistanceProfile
{
    [JsonPropertyName("Physical")]
    public double Physical { get; set; }

    [JsonPropertyName("Fire")]
    public double Fire { get; set; }

    [JsonPropertyName("Storm")]
    public double Storm { get; set; }

    [JsonPropertyName("Frost")]
    public double Frost { get; set; }

    [JsonPropertyName("Shadow")]
    public double Shadow { get; set; }

    [JsonPropertyName("Radiant")]
    public double Radiant { get; set; }
}

public class BossPhase
{
    [JsonPropertyName("phase_number")]
    public int PhaseNumber { get; set; }

    [JsonPropertyName("hp_start_pct")]
    public double HpStartPct { get; set; }

    [JsonPropertyName("hp_end_pct")]
    public double HpEndPct { get; set; }

    [JsonPropertyName("is_execute_phase")]
    public bool IsExecutePhase { get; set; }

    [JsonPropertyName("fight_types")]
    public List<string> FightTypes { get; set; } = [];

    [JsonPropertyName("mechanic")]
    public PhaseMechanic Mechanic { get; set; } = new();
}

public class PhaseMechanic
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("damage_value")]
    public double DamageValue { get; set; }

    [JsonPropertyName("needs_interrupt")]
    public bool NeedsInterrupt { get; set; }
}
