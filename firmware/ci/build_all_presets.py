import subprocess
import sys
from pathlib import Path
from typing import List, Optional

from pydantic import BaseModel, Field, ValidationError, ConfigDict
import logging

logging.basicConfig(
    level=logging.INFO,
    format="%(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)
logger = logging.getLogger(__name__)

# -------------------------------------------------
# Pydantic Models (snake_case + JSON aliases)
# -------------------------------------------------


class BuildPreset(BaseModel):
    model_config = ConfigDict(populate_by_name=True)

    name: str
    display_name: Optional[str] = Field(
        default=None,
        validation_alias="displayName",
        serialization_alias="displayName",
    )
    description: Optional[str] = None
    configure_preset: Optional[str] = Field(
        default=None,
        validation_alias="configurePreset",
        serialization_alias="configurePreset",
    )
    hidden: bool = False


class CMakePresets(BaseModel):
    model_config = ConfigDict(populate_by_name=True)

    version: int
    build_presets: List[BuildPreset] = Field(
        default_factory=list,
        validation_alias="buildPresets",
        serialization_alias="buildPresets",
    )


# -------------------------------------------------
# Build Logic
# -------------------------------------------------

PRESETS_FILE = Path("CMakePresets.json")


def load_presets() -> CMakePresets:
    if not PRESETS_FILE.exists():
        logger.error("CMakePresets.json not found.")
        sys.exit(1)

    try:
        return CMakePresets.model_validate_json(PRESETS_FILE.read_text())
    except ValidationError as exc:
        logger.error("Invalid CMakePresets.json:")
        logger.error(exc)
        sys.exit(1)


def configure_if_needed(preset: BuildPreset) -> None:
    if preset.configure_preset:
        logger.info(
            f"Configuring preset: {preset.name} using configure preset: {preset.configure_preset}"
        )
        subprocess.run(
            ["cmake", "--preset", preset.configure_preset],
            check=True,
        )


def build_preset(preset: BuildPreset) -> None:
    print("=" * 60)
    print(f"Building preset: {preset.name}")
    print("=" * 60)

    result = subprocess.run(["cmake", "--build", "--preset", preset.name])

    if result.returncode != 0:
        logger.error(f"\nBuild failed for preset: {preset.name}")
        sys.exit(result.returncode)


def main():
    presets = load_presets()

    if not presets.build_presets:
        logger.info("No build presets defined.")
        return

    for preset in presets.build_presets:
        if preset.hidden:
            continue
        configure_if_needed(preset)
        build_preset(preset)

    logger.info("\nAll presets built successfully.\n")


if __name__ == "__main__":
    main()
