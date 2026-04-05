using System.Text.Json.Serialization;

namespace RaidSimTools.Models;

public class PlayerFile
{
    [JsonPropertyName("players")]
    public List<Player> Players { get; set; } = [];
}

public class Player
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("class")]
    public string Class { get; set; } = "";

    [JsonPropertyName("spec")]
    public string Spec { get; set; } = "";

    [JsonPropertyName("item_level")]
    public double ItemLevel { get; set; }

    [JsonPropertyName("performance_rating")]
    public double PerformanceRating { get; set; }

    [JsonPropertyName("attendance_percent")]
    public double AttendancePercent { get; set; }

    [JsonPropertyName("attitude")]
    public string Attitude { get; set; } = "";

    [JsonPropertyName("potential")]
    public int Potential { get; set; }

    [JsonPropertyName("max_hp")]
    public int MaxHp { get; set; }

    [JsonPropertyName("item_class")]
    public string ItemClass { get; set; } = "";

    [JsonPropertyName("items")]
    public List<PlayerItem> Items { get; set; } = [];
}

public class PlayerItem
{
    [JsonPropertyName("slot")]
    public string Slot { get; set; } = "";

    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("crit")]
    public int Crit { get; set; }

    [JsonPropertyName("haste")]
    public int Haste { get; set; }

    [JsonPropertyName("expertise")]
    public int Expertise { get; set; }

    [JsonPropertyName("constitution")]
    public int Constitution { get; set; }
}
