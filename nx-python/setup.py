"""
Setup configuration for nx-python Phase 14B bindings.
"""

from setuptools import setup, find_packages

setup(
    name="nx-python",
    version="14b.0.0",
    description="Nanma NX-MediaSuite Python Bindings (CLI-Mirrored)",
    long_description="1:1 Python bindings to nx CLI. Transport layer only - no added logic.",
    packages=find_packages(),
    python_requires=">=3.8",
    install_requires=[
        # No dependencies - pure subprocess invocation
    ],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
)